
#include "app_battery.h"
#include "sys_err.h"
#include "display.h"
#include "bt.h"

#define TAG "APP_BATTERY"

static battery_handle_t battery;
static bool app_bat_lp_flag = false; // Battery low power flag

static void app_battery_lp(void);

void app_battery_init(void)
{
    battery_config_t battery_config = {
        .adc = ADC2,
        .adc_timeout_ms = 10,
        .min = 3080, // 2874 - 3.0V
        .max = 4080  // 4095 - 4.2V
    };
    battery_init(&battery, &battery_config);
}

void app_battery_task(void)
{
    static bool lp_last = false;

    battery_task(&battery);
    app_bat_lp_flag = (battery_get_level(&battery) < 20);

    if (app_bat_lp_flag && !lp_last) {
        app_battery_lp();
    }
    lp_last = app_bat_lp_flag;
}

u8 app_battery_get_level(void)
{
    return battery_get_level(&battery);
}

bool app_battery_is_low_power(void)
{
    return app_bat_lp_flag;
}

static void app_battery_lp(void)
{
}

void app_battery_report_lp_task(void)
{
    if (app_bat_lp_flag) {
        bt_printf("Battery low power: %d%%\r\n", battery_get_level(&battery));
    }
}
