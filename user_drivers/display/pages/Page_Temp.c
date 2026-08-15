/**
 * @file Page_Temp.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 页面编写示例。使用时可以在编辑器里将 Temp 字样统一替换成自己页面的名字。
 * 
 * @version 0.1
 * @date 2025-07-21
 * 
 */
#include "Page_Temp.h"
#include "temp.h"
#include "dht11.h"
#include "app_clock.h"

void Page_Temp_Subtrate(void)
{
    oled_show_icon16(OLED_ROW(1), OLED_COL(1), Icon16_TEMPERATURE);
    oled_show_string(OLED_ROW(1), OLED_COL(4) - 4, "Temp");
    oled_show_char(OLED_ROW(1), OLED_COL(14), ':');

    oled_show_string(4, OLED_COL(10) + 3, "`C");

    oled_show_icon16(OLED_ROW(2)+1, OLED_COL(14) + 4, Icon16_WATER_DROP);
    oled_show_char(OLED_ROW(3)+1, OLED_COL(16), '%');
}

void Page_Temp_Task(void)
{
    s8 t_int_tmp = t_int;
    u8 t_deci_tmp = t_deci_1;
    oled_show_int(OLED_ROW(1), OLED_COL(12), Calendar.hour, 2, true);
    oled_show_int(OLED_ROW(1), OLED_COL(15), Calendar.min, 2, true);
    if (t_int_tmp < 0) {
        t_int_tmp = -t_int_tmp;
        if (t_int_tmp >= 10) {
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(1), 0, FONT_DASH3216);
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(3), t_int_tmp / 10, FONT_NUM3216_1);
        } else {
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(1), 0, FONT_BLANK3216);
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(3), 0, FONT_DASH3216);
        }
    } else {
        oled_show_num3216(OLED_ROW(3)-1, OLED_COL(1), 0, FONT_BLANK3216);
        if (t_int_tmp >= 10) {
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(3), t_int_tmp / 10, FONT_NUM3216_1);
        } else {
            oled_show_num3216(OLED_ROW(3)-1, OLED_COL(3), 0, FONT_BLANK3216);
        }
    }
    oled_show_num3216(OLED_ROW(3)-1, OLED_COL(5), t_int_tmp % 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3)-1, OLED_COL(7), 0, FONT_DOT3216);
    oled_show_num3216(OLED_ROW(3)-1, OLED_COL(8), t_deci_tmp, FONT_NUM3216_1);

    if (hum_error) {
        oled_show_string(OLED_ROW(3)+1, OLED_COL(14), "--");
    } else {
        oled_show_int(OLED_ROW(3)+1, OLED_COL(14), hum_int, 2, false);
    }
}
