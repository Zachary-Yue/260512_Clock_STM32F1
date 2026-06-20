
#pragma once

#include "oled/i2c_oled.h"
#include "pages/pages.h"
#include "fonts/fonts.h"

#define DISPLAY_EN_REINIT 0
#define DISPLAY_REINIT_REFUSE_STR "Display reinit function is disabled."

typedef enum page_enum
{
    PAGE_Main = 0,
    PAGE_Cldr,
    PAGE_Alarm,
    PAGE_Timer,
    PAGE_Watch,

    PAGE_TOTAL
} page_enum;

extern page_enum curr_page;
extern i2c_oled_t oled;

#define display_scr_is_on() (oled.scr_is_on)

void display_init(void);
void display_subtrate(void);
void display_task(void);
void display_scr_on_off(bool on);
void display_reinit_timer(void);
u32  display_set_reinit_interval_h(u32 interval_h);
u32  display_get_reinit_interval_h(void);
u32  display_get_i2c_error_code(void);
u32  display_get_i2c_state(void);
