
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


    DISPLAY_I2C_DEINIT();
    OLED_POWER_OFF();
    LL_mDelay(100);
    OLED_POWER_ON();
    LL_mDelay(10);
    DISPLAY_I2C_INIT();

    i2c_oled_cfg_t cfg = {
        .display_buf = disp_buf,
        .i2c_mem_write_block = i2c_mem_write_block,
        .i2c_mem_write_dma = i2c_mem_write_dma,
        .display_buf_size = sizeof(disp_buf),
        .width = 128,
        .height = 64,
        .i2c_addr = 0x78
    };
    if (i2c_oled_init(&oled, &cfg)) {
        LOGI(TAG, "OLED initialized successfully");
        display_subtrate();
    } else {
        LOGE(TAG, "Failed to initialize OLED");
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
