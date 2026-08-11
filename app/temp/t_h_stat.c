/**
 * @file t_h_stat.c
 * @author Zach (zachary-yue@qq.com)
 *
 * @brief 温度/湿度历史统计与曲线显示。每 30 分钟存一条,环形缓冲保存 96 条(2 天)。
 *
 * @note  - 数据由 RTC 中断路径(Clock_Inc1s_Handler → t_h_stat_store_1min)写入,
 *         打印任务只能先做一次性快照再读取,避免读到撕裂的数据
 *          - 曲线通过 BT 串口逐行输出,每行之间等待发送 FIFO 腾出空间,
 *         防止 dx_bt37_printf 内部因 FIFO 满而截断丢数据
 *          - 起点时间 = 最旧数据的时间(未写满时为第一条数据的时间,写满后随覆盖推进),
 *         终点时间 = 最新数据的时间
 *
 * @version 0.2
 * @date 2026-08-10
 *
 */
#include "t_h_stat.h"
#include "temp.h"
#include "dht11.h"
#include "app_clock.h"
#include "bt.h"
#include "os_task.h"
#include "user_fifo.h"
#include "user_tick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T_H_STAT_SIZE 96 // Two days, every 30 minutes

#define show_printf(fmt, ...) bt_printf(fmt, ##__VA_ARGS__)

// 打印前等待发送 FIFO 至少腾出这么多字节,保证一行(约 80 字节)能完整写入
#define T_H_TX_FREE 250

// 相邻两行的最小输出间隔(ms)。模块一次最多处理 ~160B,多发要主动停顿;
// UART 为 115200 波特率,单行 ~80B 约 7ms 发完,20ms 间隔给模块留出处理时间。
// 实测(115200, 20ms/行)链路稳定无丢行
#define T_H_TX_MIN_PERIOD 20

typedef struct
{
    s8 ti; // Temperature integer part
    u8 td; // Temperature decimal part
    u8 hi; // Humidity integer part
} t_h_stat_t;

static t_h_stat_t t_h_stat[T_H_STAT_SIZE]; // 环形缓冲,写位置为 t_h_stat_idx
static t_h_stat_t t_h_snap[T_H_STAT_SIZE]; // 打印时的一次性快照
static u8 t_h_stat_idx = 0, t_h_stat_cnt = 0;
static u8 h_start = 0, min_start = 0, day_start = 1; // 保存的最早的数据的时间
static u8 h_curr = 0, min_curr = 0, day_curr = 1;    // 最新的数据的时间

// 打印时的快照(关中断一次性拷贝,避免与 RTC 中断的存储竞争)
static u8 snap_cnt = 0, snap_idx = 0;
static u8 snap_h_start, snap_min_start, snap_day_start;
static u8 snap_h_curr, snap_min_curr, snap_day_curr;

// 打印任务的行状态(跨挂起保持)
static u16 th_k = 0;        // 当前行号:0..2*snap_cnt-1,前半是温度,后半是湿度
static u8 th_h, th_m, th_d; // 当前行的时间(从最旧数据的时间开始,每行推进 30 分钟)
static u32 th_ts = 0;       // 最近一次成功输出的 tick,发送卡死时超时跳出
static float t_scale;       // 温度曲线缩放系数(默认 1 = 0.25°C/字符)
static s8 t_min_i;          // 温度整数部分最小值(曲线左基准)
static u8 t_base;           // 最小值对应的横线列(默认 20,超宽时收缩到 2)

bool t_h_show_flag = false;

void t_h_stat_init(void)
{
    memset(t_h_stat, 0, sizeof(t_h_stat));
    t_h_stat_idx = 0;
    t_h_stat_cnt = 0;
    h_start = 0; min_start = 0; day_start = 1;
    h_curr = 0; min_curr = 0; day_curr = 1;
}

// 时间推进 30 分钟(支持跨日)
static void t_h_advance_30min(u8 *h, u8 *m, u8 *d)
{
    *m += 30;
    if (*m >= 60)
    {
        *m -= 60;
        (*h)++;
        if (*h >= 24)
        {
            *h -= 24;
            (*d)++;
        }
    }
}

void t_h_stat_store_1min(void)
{
    if (Calendar.min != 0 && Calendar.min != 30) return;

    // 第一条数据时记录起点时间;写满后每覆盖一条,起点同步推进 30 分钟
    if (t_h_stat_cnt == 0)
    {
        h_start = Calendar.hour;
        min_start = Calendar.min;
        day_start = Calendar.day;
    }

    t_h_stat[t_h_stat_idx].ti = t_int;
    t_h_stat[t_h_stat_idx].td = t_deci_1;
    t_h_stat[t_h_stat_idx].hi = hum_int;
    h_curr = Calendar.hour;
    min_curr = Calendar.min;
    day_curr = Calendar.day;

    t_h_stat_idx++;
    if (t_h_stat_idx >= T_H_STAT_SIZE)
        t_h_stat_idx = 0;
    if (t_h_stat_cnt < T_H_STAT_SIZE)
    {
        t_h_stat_cnt++;
    }
    else
    {
        t_h_advance_30min(&h_start, &min_start, &day_start);
    }
}

// 按时间顺序取快照中第 k 个(0 = 最旧)样本的下标。环形缓冲写满后最旧数据在 snap_idx
static u8 t_h_snap_at(u8 k)
{
    u8 i = (snap_cnt == T_H_STAT_SIZE) ? (u8)(snap_idx + k) : k;
    if (i >= T_H_STAT_SIZE) i -= T_H_STAT_SIZE;
    return i;
}

// 计算温度曲线显示参数(湿度曲线按 2%/字符直接映射,无需参数)。
// 以温度整数部分的最小/最大值为基准,显示范围 = (max+1)-min 度,默认 1 字符 = 0.25°C。
// 显示范围(极差)限制在 [8, 48] 字符,超出则整体缩放;最小值默认映射到第 20 列,
// 若 20 + 极差 > 50,则把 max+1 映射到第 50 列(最小值随之收缩到 2 ~ 20 列)
static void t_h_curve_calc(void)
{
    s8 min_i = 0, max_i = 0;
    for (u8 k = 0; k < snap_cnt; k++)
    {
        s8 v = t_h_snap[t_h_snap_at(k)].ti;
        if (k == 0)
        {
            min_i = max_i = v;
            continue;
        }
        if (v < min_i) min_i = v;
        if (v > max_i) max_i = v;
    }

    float range_chars = (float)((int)max_i + 1 - (int)min_i) * 4.0f; // (max+1-min)°C × 4 字符/°C
    float scaled = range_chars;
    if (range_chars < 8.0f)       scaled = 8.0f;  // 极差 < 2°C:放大到 8 字符
    else if (range_chars > 48.0f) scaled = 48.0f; // 极差 > 12°C:压缩到 48 字符
    t_scale = scaled / range_chars;               // range_chars >= 4,不会除零
    t_base = (20.0f + scaled <= 50.0f) ? 20 : (u8)(50.0f - scaled + 0.5f);
    t_min_i = min_i;
}

// 打印一行样本: 时间 + 曲线条 + 数值标注 + (零点日期标记)。
// 曲线条最多 50 字符,数值标注紧跟在横线后面(空一格);日期固定写在第 62(湿度)/66(温度)列,
// 即 50 字符曲线区 + 4/8 字符标注块之后,与横线长度无关
static void t_h_print_line(u8 h, u8 m, u8 d, bool day_mark, u8 pos, int v_int, int v_dec, bool is_hum)
{
    char line[96];
    u16 n = (u16)snprintf(line, sizeof(line), "%02d:%02d |", h, m);
    for (u8 i = 0; i < pos; i++)
        line[n++] = '-';

    if (is_hum)
        n += (u16)snprintf(line + n, sizeof(line) - n, " %2d%%", v_int);
    else
        n += (u16)snprintf(line + n, sizeof(line) - n, " %d.%d°C", v_int, v_dec);

    if (day_mark)
    {
        u8 date_col = is_hum ? 62 : 66;
        while (n < date_col - 1) line[n++] = ' ';
        n += (u16)snprintf(line + n, sizeof(line) - n, "  %d 日", d);
    }
    line[n] = '\0';
    show_printf("%s\r\n", line);
}

// 打印任务: 逐行输出温度/湿度曲线,每行之间等待发送 FIFO 腾出空间
void t_h_stat_show_task(void)
{
    task_start(TASK_T_H_STAT_SHOW);
    task_wait_until(t_h_show_flag);
    t_h_show_flag = false;

    if (t_h_stat_cnt == 0)
    {
        show_printf("\r\n暂无温度/湿度统计数据\r\n");
        show_printf("\r\nClock> ");
    }
    else
    {
        // 一次性快照:打印期间 RTC 中断仍会写入环形缓冲
        __disable_irq();
        snap_cnt = t_h_stat_cnt;
        snap_idx = t_h_stat_idx;
        snap_h_start = h_start;  snap_min_start = min_start;  snap_day_start = day_start;
        snap_h_curr  = h_curr;   snap_min_curr  = min_curr;   snap_day_curr  = day_curr;
        memcpy(t_h_snap, t_h_stat, sizeof(t_h_snap));
        __enable_irq();

        t_h_curve_calc();

        th_k = 0;
        th_h = snap_h_start; th_m = snap_min_start; th_d = snap_day_start;
        th_ts = user_get_tick();

        // 逐行输出:每行等待发送 FIFO 腾出空间且距上一行足够久(限速到链路可承受的 ~420B/s),
        // 或全部行已打完,再打印一行。挂起后由 state 记录在本行,恢复时继续处理同一行
        task_wait_until(th_k >= (u16)snap_cnt * 2
                        || (user_fifo_get_used_length(&bt37.tx_fifo) < bt37.tx_fifo.max_size - T_H_TX_FREE
                            && (u32)(user_get_tick() - th_ts) >= T_H_TX_MIN_PERIOD));

        if (th_k >= (u16)snap_cnt * 2)
        {
            show_printf("\r\nClock> ");
            // 全部行已输出:复位任务,等待下一次触发
            user_tasks_[this_task_].state = 0;
        }
        else if ((u32)(user_get_tick() - th_ts) > 3000u)
        {
            // 发送卡死超时(3s 未排出数据):放弃剩余输出,复位任务。
            // 不能在这里继续打印——FIFO 已满,写入会被静默丢弃,只会得到残缺输出
            user_tasks_[this_task_].state = 0;
        }
        else
        {
            // 曲线标题
            if (th_k == 0)
                show_printf("\r\n\r\n--- %d 日 %d:%02d ~ %d 日 %d:%02d 的温度曲线 ---\r\n\r\n",
                            snap_day_start, snap_h_start, snap_min_start,
                            snap_day_curr, snap_h_curr, snap_min_curr);
            else if (th_k == (u16)snap_cnt)
            {
                // 湿度曲线起点:时间回到最旧数据
                th_h = snap_h_start; th_m = snap_min_start; th_d = snap_day_start;
                show_printf("\r\n\r\n--- %d 日 %d:%02d ~ %d 日 %d:%02d 的湿度曲线 ---\r\n\r\n",
                            snap_day_start, snap_h_start, snap_min_start,
                            snap_day_curr, snap_h_curr, snap_min_curr);
            }

            // 输出一行样本。时间标签从起点逐行推进 30 分钟
            if (th_k < (u16)snap_cnt)
            {
                u8 i = t_h_snap_at((u8)th_k);
                s8 ti = t_h_snap[i].ti;
                u8 td = t_h_snap[i].td;
                float rel = ((float)((int)ti - (int)t_min_i) + (float)td * 0.1f) * 4.0f * t_scale;
                u8 pos = (u8)(t_base + rel + 0.5f);
                if (pos < 2) pos = 2;
                if (pos > 50) pos = 50;
                t_h_print_line(th_h, th_m, th_d,
                               th_h == 0 && th_m == 0,
                               pos, ti, td, false);
            }
            else
            {
                u8 i = t_h_snap_at((u8)(th_k - (u16)snap_cnt));
                u8 pos = (u8)((float)t_h_snap[i].hi * 0.5f + 0.5f);
                if (pos > 50) pos = 50;
                t_h_print_line(th_h, th_m, th_d,
                               th_h == 0 && th_m == 0,
                               pos, t_h_snap[i].hi, 0, true);
            }

            t_h_advance_30min(&th_h, &th_m, &th_d);
            th_k++;
            th_ts = user_get_tick();
        }
        break; // 返回主循环;下一行由上面的节流等待继续驱动(状态已保存在该等待点)
    }

    task_end(1);
}

// 测试函数:随机装入一批温度/湿度数据并触发曲线打印,用于查看打印效果。
// 注意:会清空已保存的真实统计数据。
void t_h_stat_test(void)
{
    srand(user_get_tick()); // 每次调用都产生不同的数据

    // 一半概率写满 96 条(写位置随机,验证环形缓冲写满+环绕的读取顺序);
    // 其余随机 8~95 条(验证未写满时的起点时间标签)
    u8 cnt = (rand() & 1) ? T_H_STAT_SIZE : (u8)(8 + rand() % 88);
    u8 idx = (cnt == T_H_STAT_SIZE) ? (u8)(rand() % T_H_STAT_SIZE) : cnt;

    // 随机选择数据范围,覆盖曲线显示的各种分支:
    //   0: 20~30°C / 40~60%  极差 44 字符 > 30,max+1 收进第 50 列
    //   1: 24~24°C / 55~60%  极差 4 字符 < 8,放大到 8 字符(整数部分相同)
    //   2: -5~35°C / 10~90%  极差 164 字符 > 48,压缩到 48 字符,横线最短 2 字符
    //   3: 25~30°C / 40~60%  极差 24 字符,默认从第 20 列起画
    //   4: -20~5°C / 10~90%  负温标注 + 压缩到 48 字符
    u8 sc = (u8)(rand() % 5);
    s8 t_lo, t_hi;
    u8 h_lo, h_hi;
    switch (sc)
    {
        case 0: t_lo = 20; t_hi = 30; h_lo = 40; h_hi = 60; break;
        case 1: t_lo = 24; t_hi = 24; h_lo = 55; h_hi = 60; break;
        case 2: t_lo = -5; t_hi = 35; h_lo = 10; h_hi = 90; break;
        case 3: t_lo = 25; t_hi = 30; h_lo = 40; h_hi = 60; break;
        default: t_lo = -20; t_hi = 5; h_lo = 10; h_hi = 90; break;
    }
    u8 t_span = (u8)(t_hi - t_lo + 1);
    u8 h_span = (u8)(h_hi - h_lo + 1);

    t_h_stat_init();

    // 起点时间:任意半小时
    h_start = (u8)(rand() % 24);
    min_start = (u8)((rand() & 1) ? 30 : 0);
    day_start = 1 + (u8)(rand() % 20);

    // 装入随机数据。与真实存储布局一致:写满时第 k 条(0=最旧)在下标 (idx+k)%96,未写满时在下标 k
    __disable_irq();
    for (u8 k = 0; k < cnt; k++)
    {
        u8 i = (cnt == T_H_STAT_SIZE) ? (u8)((idx + k) % T_H_STAT_SIZE) : k;
        t_h_stat[i].ti = (s8)(t_lo + rand() % t_span);
        t_h_stat[i].td = (u8)(rand() % 10);
        t_h_stat[i].hi = (u8)(h_lo + rand() % h_span);
    }
    t_h_stat_cnt = cnt;
    t_h_stat_idx = idx;
    __enable_irq();

    // 终点时间 = 起点 + cnt 个 30 分钟
    h_curr = h_start; min_curr = min_start; day_curr = day_start;
    for (u8 k = 0; k < cnt; k++)
        t_h_advance_30min(&h_curr, &min_curr, &day_curr);

    show_printf("t_h_stat_test: %d 条, 场景 %d (%d~%d°C, %d~%d%%), %d日 %d:%02d ~ %d日 %d:%02d\r\n",
                cnt, sc, (int)t_lo, (int)t_hi, h_lo, h_hi,
                day_start, h_start, min_start, day_curr, h_curr, min_curr);
    t_h_show_flag = true; // 触发 t_h_stat_show_task 逐行打印
}
