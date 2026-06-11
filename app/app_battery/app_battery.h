#pragma once

#include "battery.h"

void app_battery_init(void);
void app_battery_task(void);
u8   app_battery_get_level(void);
bool app_battery_is_low_power(void);
void app_battery_report_lp_task(void);
