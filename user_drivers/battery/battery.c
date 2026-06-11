#include "battery.h"
#include "user_tick.h"
#include "sys_err.h"

#define TAG "BATTERY"

u8 battery_get_level(battery_handle_t *handle)
{
    CHECK_FALSE_RET(handle, 0);
    return handle->level;
}

// Get current battery level by ADC
static bool battery_adc_get_value(battery_handle_t *handle, u8 *level)
{
    CHECK_FALSE_RET(handle && level, false);
    u16 tmp = 0;
    u32 tick_start = user_get_tick();
    LL_ADC_ClearFlag_EOS(handle->adc);
    LL_ADC_REG_StartConversionSWStart(handle->adc);
    while (!LL_ADC_IsActiveFlag_EOS(handle->adc)) {
        if (user_get_tick() - tick_start > handle->adc_timeout_ms) { // 超时
            return false;
        }
    }
    tmp = LL_ADC_REG_ReadConversionData12(handle->adc);
    tmp > handle->max ? tmp = handle->max : (tmp < handle->min ? tmp = handle->min : 0);
    *level = (tmp - handle->min) * 100 / (handle->max - handle->min);
    return true;
}

// It's suggested to call every 5 s.
void battery_task(battery_handle_t *handle)
{
    CHECK_FALSE_RET_VOID(handle);
    static u8 call_cnt = 0;

    u8 battery_level = 0;
    if (!battery_adc_get_value(handle, &battery_level)) {
        LOGE(TAG, "Failed to get battery level.");
        return;
    }

    handle->past[handle->past_idx] = battery_level;
    handle->past_idx = (handle->past_idx + 1) % sizeof(handle->past);

    if (call_cnt < sizeof(handle->past)) {
        u16 sum = 0;
        ++call_cnt;
        for (u8 i = 0; i < call_cnt; ++i) {
            sum += handle->past[i];
        }
        handle->level = sum / call_cnt;
    }
    else {
        u16 sum = 0;
        for (u8 i = 0; i < sizeof(handle->past); ++i) {
            sum += handle->past[i];
        }
        // handle->level = sum / sizeof(handle->past);
        handle->level = sum >> 3; // 除以8等价于右移3位
    }
}

void battery_init(battery_handle_t *handle, const battery_config_t *config)
{
    CHECK_FALSE_RET_VOID(handle && config);
    handle->adc = config->adc;
    handle->adc_timeout_ms = config->adc_timeout_ms;
    handle->min = config->min;
    handle->max = config->max;
    handle->level = 100;
    handle->past_idx = 0;
    memset(handle->past, 100, sizeof(handle->past));
    LL_ADC_Enable(handle->adc);
}
