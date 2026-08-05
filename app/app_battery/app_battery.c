
#include "app_battery.h"
#include "sys_err.h"
#include "display.h"
#include "bt.h"
#include "os_task.h"

#define TAG "APP_BATTERY"

static battery_handle_t battery;
static bool app_bat_lp_flag = false; // Battery low power flag
static bool lp_last = false;         // 上次采样的低电量标志（边沿检测用，跨任务挂起保持）

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
    task_start(TASK_BATTERY);

    task_while(1)
    {
        task_delay(5000);

        battery_task(&battery);
        app_bat_lp_flag = (battery_get_level(&battery) < 20);

        if (app_bat_lp_flag && !lp_last) {
            app_battery_lp();
        }
        lp_last = app_bat_lp_flag;
    }
    task_loop(1);
    task_end(0);
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
    task_start(TASK_BATTERY_REPORT_LP);

    task_while(1)
    {
        task_wait(app_bat_lp_flag);   // 低电标志置位才继续
        bt_printf("Battery low power: %d%%\r\n", battery_get_level(&battery));
        task_delay(10000);            // 10s 节流
    }
    task_loop(1);
    task_end(0);
}
