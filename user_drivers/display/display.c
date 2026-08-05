
#include "display.h"
#include "user_phr.h"
#include "user_i2c.h"
#include "user_tick.h"
#include "os_task.h"
#include "sys_err.h"
#include "i2c.h"

#define TAG "DISPLAY"

#define OLED_POWER_PIN PAout(7)
#define OLED_POWER_ON() (OLED_POWER_PIN = Bit_RESET)
#define OLED_POWER_OFF() (OLED_POWER_PIN = Bit_SET)

// 一帧 DMA 传输超过该时间未完成，视为 I2C 卡死（错误中止后完成回调不再触发）
#define DISPLAY_FRAME_TIMEOUT_MS (1000)

page_enum curr_page = PAGE_Main;
i2c_oled_t oled;
static u8 disp_buf[1024];

u8 disp_fps = 0; // 上一秒成功启动刷新的帧数（调试备用，未显示）
static u8 disp_fps_cnt = 0;
static u32 refresh_start = 0;              // 帧看门狗截止时刻
static bool f_oled_set_on_off_req = false; // 屏幕开关请求标志（总线忙时排队）
static bool f_oled_set_on_off = false;     // 请求的目标状态

static bool i2c_mem_write_block(u8 slvaddr, u8 memaddr, u8 *data, u16 len)
{
    CHECK_FALSE_RET(i2c_mem_write_start(&hi2c1, slvaddr, memaddr, data, len), false);
    return i2c_send_wait(&hi2c1, 1000);
}

static bool i2c_mem_write_dma(u8 slvaddr, u8 memaddr, u8 *data, u16 len)
{
    return i2c_mem_write_start(&hi2c1, slvaddr, memaddr, data, len);
}

void display_subtrate(void)
{
    i2c_oled_clear(&oled);

    switch (curr_page)
    {
    default:
        break;

    case PAGE_Main:
        Page_Main_Subtrate();
        break;
    case PAGE_Cldr:
        Page_Cldr_Subtrate();
        break;
    case PAGE_Alarm:
        Page_Alarm_Subtrate();
        break;
    case PAGE_Timer:
        Page_Timer_Subtrate();
        break;
    case PAGE_Watch:
        Page_Watch_Subtrate();
        break;
    case PAGE_TEMP:
        Page_Temp_Subtrate();
        break;
    }
}

/**
 * @brief I2C1 传输完成回调（I2C1_EV 中断上下文，只清一个 volatile bool）。
 *        供 display_task 帧同步：一帧 DMA 刷新完成后允许画下一帧。
 */
static void display_i2c_send_cplt(I2C_Send_t *hi2c)
{
    USER_UNUSED(hi2c);
    oled.is_refreshing = false;
}

void display_task(void)
{
    task_start(TASK_DISPLAY);

    task_delay(50); // 上电/总线稳定后再初始化
    display_init(); // 初始化 + 画衬底（只执行一次）

    task_while(1)
    {
        /* 帧同步：上一帧 DMA 完成才画下一帧；
           超过 1s 未完成（I2C 错误中止后完成回调不再触发）→ 强制继续 */
        task_wait(!oled.is_refreshing ||
                  (u32)(user_get_tick() - refresh_start) >= DISPLAY_FRAME_TIMEOUT_MS);

        if (oled.is_refreshing)
        {
            /* 看门狗超时：DMA 卡死 → 电源循环复位（display_init 会清 is_refreshing） */
            LOGW(TAG, "Display frame timeout, reinitializing...");
            display_init();
            task_continue(); /* 回到循环头，下一轮正常刷新 */
        }

        if (!oled.is_init)
        {
            /* 初始化失败（OLED 未上电/未连接）：每秒重试，保留自愈能力 */
            task_delay(1000);
            display_init();
            task_continue();
        }

        /* 屏幕开关请求：总线空闲时排队执行（1 字节命令，ms 级） */
        if (f_oled_set_on_off_req)
        {
            f_oled_set_on_off_req = false;
            i2c_oled_scr_on_off(&oled, f_oled_set_on_off);
        }

        if (display_scr_is_on())
        {
            switch (curr_page)
            {
            default:
                break;

            case PAGE_Main:
                Page_Main_Task();
                break;
            case PAGE_Cldr:
                Page_Cldr_Task();
                break;
            case PAGE_Alarm:
                Page_Alarm_Task();
                break;
            case PAGE_Timer:
                Page_Timer_Task();
                break;
            case PAGE_Watch:
                Page_Watch_Task();
                break;
            case PAGE_TEMP:
                Page_Temp_Task();
                break;
            }
            if (i2c_oled_refresh(&oled))
            {
                refresh_start = user_get_tick();
                disp_fps_cnt++;
            }
            else
            {
                task_delay(100); /* 启动失败退避（总线忙），避免快速重试风暴 */
            }
        }
    }
    task_loop(1);
    task_end(0);
}

void display_fps_task(void)
{
    task_start(TASK_DISPLAY_FPS);

    task_while(1)
    {
        task_delay(1000);
        disp_fps = disp_fps_cnt;
        disp_fps_cnt = 0;
    }
    task_loop(1);
    task_end(0);
}

void display_init(void)
{
    LOGI(TAG, "Initializing display...");

    i2c_send_abort(&hi2c1);
    oled.is_refreshing = false; // 在途帧可能被上面的 abort 终止，完成回调不再触发
    i2c1_reset();
    OLED_POWER_OFF();
    LL_mDelay(20);
    OLED_POWER_ON();
    LL_mDelay(10);

    i2c_oled_cfg_t cfg = {
        .display_buf = disp_buf,
        .i2c_mem_write_block = i2c_mem_write_block,
        .i2c_mem_write_dma = i2c_mem_write_dma,
        .display_buf_size = sizeof(disp_buf),
        .width = 128,
        .height = 64,
        .i2c_addr = 0x78,
    };
    if (i2c_oled_init(&oled, &cfg))
    {
        i2c_send_setcb(display_i2c_send_cplt); // 注册传输完成回调（幂等）
        LOGI(TAG, "Display initialized successfully");
        display_subtrate();
    }
    else
    {
        LOGE(TAG, "Failed to initialize display");
    }
}

void display_scr_on_off(bool on)
{
    /* 只置标志排队，实际写屏由 display_task 在总线空闲时执行 */
    f_oled_set_on_off = on;
    f_oled_set_on_off_req = true;
}

#if DISPLAY_EN_REINIT
static u32 display_reinit_interval_s = 0; // Off by default
static u32 display_reinit_cnt = 0;
#endif

u32 display_set_reinit_interval_h(u32 interval_h)
{
#if DISPLAY_EN_REINIT
    interval_h > (u32)(-1) / 3600 ? (interval_h = (u32)(-1) / 3600) : (void)0;
    display_reinit_interval_s = interval_h * 3600;
    display_reinit_cnt = 0;
    return interval_h;
#else
    USER_UNUSED(interval_h);
    LOGE(TAG, DISPLAY_REINIT_REFUSE_STR);
    return 0;
#endif
}

u32 display_get_reinit_interval_h(void)
{
#if DISPLAY_EN_REINIT
    return display_reinit_interval_s / 3600;
#else
    LOGE(TAG, DISPLAY_REINIT_REFUSE_STR);
    return 0;
#endif
}

u32 display_get_i2c_error_code(void)
{
    return hi2c1.ErrorCode;
}

u32 display_get_i2c_state(void)
{
    return hi2c1.State;
}
