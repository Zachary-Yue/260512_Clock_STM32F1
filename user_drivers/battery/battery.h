#pragma once

#include "main.h"

typedef struct battery_handle_t {
    ADC_TypeDef *adc;
    u8 past[8];
    u16 min;
    u16 max;
    u8 past_idx;
    u8 adc_timeout_ms;
    u8 level;
} battery_handle_t;

typedef struct battery_config_t {
    ADC_TypeDef *adc;
    u16 min;
    u16 max;
    u8 adc_timeout_ms;
} battery_config_t;

void battery_init(battery_handle_t *handle, const battery_config_t *config);
u8   battery_get_level(battery_handle_t *handle);
void battery_task(battery_handle_t *handle);
