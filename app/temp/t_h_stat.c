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
static float t_vmin, t_scale; // 温度曲线显示参数
static u8 t_line_min;
static float h_vmin, h_scale; // 湿度曲线显示参数
static u8 h_line_min;

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

static float t_h_snap_temp(u8 i) { return (float)t_h_snap[i].ti + (float)t_h_snap[i].td * 0.1f; }
static float t_h_snap_hum(u8 i)  { return (float)t_h_snap[i].hi; }

// 计算曲线显示参数。
// chars_per_unit:每字符对应的数值(温度 2 = 0.5°C/字符,湿度 1 = 1%/字符)
// 数值跨度映射到 [8, 63] 字符,超出则整体缩放;线段左边界固定 2 字符缩进,居中显示
static void t_h_curve_calc(u8 chars_per_unit, float (*get_v)(u8),
                           float *vmin, float *scale, u8 *line_min)
{
    float vmax = 0.0f;
    *vmin = 0.0f;
    for (u8 k = 0; k < snap_cnt; k++)
    {
        float v = get_v(t_h_snap_at(k));
        if (k == 0)
        {
            *vmin = vmax = v;
            continue;
        }
        if (v < *vmin) *vmin = v;
        if (v > vmax) vmax = v;
    }

    float raw_range = (vmax - *vmin) * (float)chars_per_unit; // 数值跨度对应的字符数
    float range = raw_range;
    if (raw_range <= 0.0f)      { *scale = 1.0f;             range = 8.0f; }
    else if (raw_range < 8.0f)  { *scale = 8.0f / raw_range; range = 8.0f; }
    else if (raw_range > 63.0f) { *scale = 63.0f / raw_range; range = 63.0f; }
    else                        { *scale = 1.0f; }
    *line_min = (u8)(2.0f + 32.0f - range * 0.5f + 0.5f);
}

// 打印一行样本: 时间 + 数值 + 曲线条 + (日期标记)
static void t_h_print_line(u8 h, u8 m, u8 d, bool day_mark, u8 pos, int v_int, int v_dec, bool is_hum)
{
    char bar[66];
    memset(bar, '-', pos);
    memset(bar + pos, ' ', 65 - pos);
    bar[65] = '\0';

    if (is_hum)
        show_printf("%2d:%02d  %2d%% |%s|", h, m, v_int, bar);
    else
        show_printf("%2d:%02d  %2d.%d°C |%s|", h, m, v_int, v_dec, bar);

    if (day_mark) show_printf(" %d 日", d);
    show_printf("\r\n");
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

        t_h_curve_calc(2, t_h_snap_temp, &t_vmin, &t_scale, &t_line_min);
        t_h_curve_calc(1, t_h_snap_hum,  &h_vmin, &h_scale, &h_line_min);

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
                float v = (float)t_h_snap[i].ti + (float)t_h_snap[i].td * 0.1f;
                u8 pos = (u8)(t_line_min + (v - t_vmin) * 2.0f * t_scale + 0.5f);
                if (pos > 65) pos = 65;
                t_h_print_line(th_h, th_m, th_d,
                               th_k == 0 || (th_h == 0 && th_m == 0),
                               pos, t_h_snap[i].ti, t_h_snap[i].td, false);
            }
            else
            {
                u8 i = t_h_snap_at((u8)(th_k - (u16)snap_cnt));
                u8 pos = (u8)(h_line_min + ((float)t_h_snap[i].hi - h_vmin) * h_scale + 0.5f);
                if (pos > 65) pos = 65;
                t_h_print_line(th_h, th_m, th_d,
                               th_k == (u16)snap_cnt || (th_h == 0 && th_m == 0),
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

    t_h_stat_init();

    // 起点时间:任意半小时
    h_start = (u8)(rand() % 24);
    min_start = (u8)((rand() & 1) ? 30 : 0);
    day_start = 1 + (u8)(rand() % 20);

    // 装入随机数据:温度 20.0~30.0°C,湿度 40~60%。
    // 与真实存储布局一致:写满时第 k 条(0=最旧)在下标 (idx+k)%96,未写满时在下标 k
    __disable_irq();
    for (u8 k = 0; k < cnt; k++)
    {
        u8 i = (cnt == T_H_STAT_SIZE) ? (u8)((idx + k) % T_H_STAT_SIZE) : k;
        t_h_stat[i].ti = (s8)(20 + (rand() % 11)); // 20~30°C
        t_h_stat[i].td = (u8)(rand() % 10);
        t_h_stat[i].hi = (u8)(40 + (rand() % 21)); // 40~60%
    }
    t_h_stat_cnt = cnt;
    t_h_stat_idx = idx;
    __enable_irq();

    // 终点时间 = 起点 + cnt 个 30 分钟
    h_curr = h_start; min_curr = min_start; day_curr = day_start;
    for (u8 k = 0; k < cnt; k++)
        t_h_advance_30min(&h_curr, &min_curr, &day_curr);

    show_printf("t_h_stat_test: %d 条, %d日 %d:%02d ~ %d日 %d:%02d\r\n",
                cnt, day_start, h_start, min_start, day_curr, h_curr, min_curr);
    t_h_show_flag = true; // 触发 t_h_stat_show_task 逐行打印
}
