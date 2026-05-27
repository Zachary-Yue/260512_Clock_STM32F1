
#include "display.h"
#include "user_phr.h"
#include "sys_err.h"
#include "i2c.h"

#define TAG "DISPLAY"

#define DISPLAY_I2C_DEINIT() do { \
    i2c_send_abort(&hi2c1); \
    LL_I2C_DeInit(I2C1); \
} while (0)
#define DISPLAY_I2C_INIT() MX_I2C1_Init()

#define OLED_POWER_PIN PAout(7)
#define OLED_POWER_ON()  (OLED_POWER_PIN = Bit_RESET)
#define OLED_POWER_OFF() (OLED_POWER_PIN = Bit_SET)

page_enum curr_page = PAGE_Main;
i2c_oled_t oled;
static u8 disp_buf[1024];

static bool i2c_mem_write_block(u8 slvaddr, u8 memaddr, u8 *data, u16 len)
{
    CHECK_FALSE_RET(i2c_mem_write_start(&hi2c1, slvaddr, memaddr, data, len), false);
    return i2c_send_wait(&hi2c1, 1000);
}

static bool i2c_mem_write_dma(u8 slvaddr, u8 memaddr, u8 *data, u16 len)
{
    return i2c_mem_write_start(&hi2c1, slvaddr, memaddr, data, len);
}

static void i2c_reset(void)
{
    display_init();
}


void display_subtrate(void)
{
    i2c_oled_clear(&oled);

    switch (curr_page)
    {
        default: break;

        case PAGE_Main: Page_Main_Subtrate(); break;
        case PAGE_Cldr: Page_Cldr_Subtrate(); break;
        case PAGE_Alarm: Page_Alarm_Subtrate(); break;
        case PAGE_Timer: Page_Timer_Subtrate(); break;
        case PAGE_Watch: Page_Watch_Subtrate(); break;
    }
}

void display_task(void)
{
    CHECK_FALSE_RET_VOID(display_scr_is_on());

    switch (curr_page)
    {
        default: break;

        case PAGE_Main: Page_Main_Task(); break;
        case PAGE_Cldr: Page_Cldr_Task(); break;
        case PAGE_Alarm: Page_Alarm_Task(); break;
        case PAGE_Timer: Page_Timer_Task(); break;
        case PAGE_Watch: Page_Watch_Task(); break;
    }

    i2c_oled_refresh(&oled);
}

void display_init(void)
{
    LOGI(TAG, "Initializing display...");

    i2c_send_abort(&hi2c1);
    i2c1_reset();
    OLED_POWER_OFF();
    LL_mDelay(10);
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
        .i2c_send_reset = i2c_reset
    };
    if (i2c_oled_init(&oled, &cfg)) {
        LOGI(TAG, "Display initialized successfully");
        display_subtrate();
    } else {
        LOGE(TAG, "Failed to initialize display");
    }
}

void display_scr_on_off(bool on)
{
    if (on) {
        i2c_oled_scr_on_off(&oled, true);
    } else {
        i2c_oled_scr_on_off(&oled, false);
    }
}

static u32 display_reinit_interval_s = 0; // Off by default
static u32 display_reinit_cnt = 0;

/* 定时重启显示屏，因为 I2C 容易死机 */
void display_reinit_timer(void)
{
    if (display_reinit_interval_s == 0) return;
    if (++display_reinit_cnt >= display_reinit_interval_s) {
        display_reinit_cnt = 0;
        LOGI(TAG, "Reinitializing display...");
        display_init();
    }
}

u32 display_set_reinit_interval_h(u32 interval_h)
{
    interval_h > (u32)(-1) / 3600 ? (interval_h = (u32)(-1) / 3600) : (void)0;
    display_reinit_interval_s = interval_h * 3600;
    display_reinit_cnt = 0;
    return interval_h;
}

u32 display_get_reinit_interval_h(void)
{
    return display_reinit_interval_s / 3600;
}

u32 display_get_i2c_error_code(void)
{
    return hi2c1.ErrorCode;
}

u32 display_get_i2c_state(void)
{
    return hi2c1.State;
}
