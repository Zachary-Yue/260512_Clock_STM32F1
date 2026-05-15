/**
 * @file Page_Watch.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 页面编写示例。使用时可以在编辑器里将 Watch 字样统一替换成自己页面的名字。
 * 
 * @version 0.1
 * @date 2025-07-21
 * 
 */
#include "Page_Watch.h"
#include "app_clock.h"


void Page_Watch_Subtrate(void)
{
    oled_show_cn16_str(OLED_ROW(1), OLED_COL(1), "秒表");
    oled_show_char(OLED_ROW(1), OLED_COL(14), ':');
    
    oled_show_num3216(OLED_ROW(3), OLED_COL(5), 0, FONT_COLON3216);
    oled_show_num3216(OLED_ROW(3), OLED_COL(11), 0, FONT_COLON3216);
}

void Page_Watch_Task(void)
{
    oled_show_int(OLED_ROW(1), OLED_COL(12), Calendar.hour, 2, true);
    oled_show_int(OLED_ROW(1), OLED_COL(15), Calendar.min, 2, true);

    oled_show_num3216(OLED_ROW(3), OLED_COL(1), Watch.hour / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(3), Watch.hour % 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(7), Watch.min / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(9), Watch.min % 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(13), Watch.sec / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(15), Watch.sec % 10, FONT_NUM3216_1);
}
