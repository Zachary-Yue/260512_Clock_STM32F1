/**
 * @file Page_Main.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 主页面的显示和按键功能。
 * 
 * @version 0.1
 * @date 2025-07-15
 * 
 */
#include "Page_Main.h"
#include "app_clock.h"
#include "app_music.h"
#include "app_battery.h"

u8 Time_FONT = FONT_NUM3216_1;

/*---------------------------------------------- User Determine -------------------------------------------*/

/**
 * @brief 主页面显示的衬底
 * 
 */
void Page_Main_Subtrate(void)
{
    oled_show_num3216(OLED_ROW(2), OLED_COL(8), 0, FONT_COLON3216);
    oled_show_char(OLED_ROW(4), OLED_COL(6), '-');
    oled_show_char(OLED_ROW(4), OLED_COL(11), '-');
}


/**
 * @brief 主页面刷新的信息
 * 
 */
void Page_Main_Task(void)
{
    oled_show_num3216(OLED_ROW(2), OLED_COL(4), Calendar.hour / 10, Time_FONT);
    oled_show_num3216(OLED_ROW(2), OLED_COL(6), Calendar.hour % 10, Time_FONT);
    oled_show_num3216(OLED_ROW(2), OLED_COL(10), Calendar.min / 10, Time_FONT);
    oled_show_num3216(OLED_ROW(2), OLED_COL(12), Calendar.min % 10, Time_FONT);
    oled_show_int(OLED_ROW(4), OLED_COL(8), Calendar.sec, 2, true);

    if (Timer_IconExistCnt) oled_show_icon16(OLED_ROW(1), OLED_COL(6), Icon16_WATCH);
    else oled_show_string(OLED_ROW(1), OLED_COL(6), "  ");
    if (app_music_is_playing()) oled_show_icon16(OLED_ROW(1), OLED_COL(8), Icon16_MUSIC_NOTE);
    else oled_show_string(OLED_ROW(1), OLED_COL(8), "  ");
    if (Alarm_IconExistCnt) oled_show_icon16(OLED_ROW(1), OLED_COL(10), Icon16_ALARM_CLOCK);
    else oled_show_string(OLED_ROW(1), OLED_COL(10), "  ");
    if (app_battery_is_low_power()) oled_show_icon16(OLED_ROW(1), OLED_COL(15), Icon16_LOW_BATTERY);
    else oled_show_string(OLED_ROW(1), OLED_COL(15), "  ");
}
