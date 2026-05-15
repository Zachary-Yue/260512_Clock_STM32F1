/**
 * @file Page_Example.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 页面编写示例。使用时可以在编辑器里将 Example 字样统一替换成自己页面的名字。
 * 
 * @version 0.1
 * @date 2025-07-21
 * 
 */
#include "Page_Cldr.h"
#include "app_clock.h"


void Page_Cldr_Subtrate(void)
{
    oled_show_cn16_str(OLED_ROW(1), OLED_COL(1), "日历");
    oled_show_char(OLED_ROW(1), OLED_COL(14), ':');
    oled_show_cn1608_str(3, OLED_COL(8), "月");
    #if 0
    oled_show_cn1608_str(OLED_ROW(3), OLED_COL(11), "日");
    #else
    oled_show_cn1608_str(3, OLED_COL(13), "日");
    #endif
}

void Page_Cldr_Task(void)
{
    oled_show_int(OLED_ROW(1), OLED_COL(12), Calendar.hour, 2, true);
    oled_show_int(OLED_ROW(1), OLED_COL(15), Calendar.min, 2, true);

    #if 0
    oled_show_int(OLED_ROW(3), OLED_COL(5), Calendar.month, 2, FILL_BY_SPACE);
    oled_show_int(OLED_ROW(3), OLED_COL(9), Calendar.day, 2, FILL_BY_SPACE);
    #else
    oled_show_num3216(OLED_ROW(2), OLED_COL(4), Calendar.month / 10, FONT_NUM3216_4);
    oled_show_num3216(OLED_ROW(2), OLED_COL(6), Calendar.month % 10, FONT_NUM3216_4);
    oled_show_num3216(OLED_ROW(2), OLED_COL(9), Calendar.day / 10, FONT_NUM3216_4);
    oled_show_num3216(OLED_ROW(2), OLED_COL(11), Calendar.day % 10, FONT_NUM3216_4);
    #endif

    oled_show_string(OLED_ROW(4), OLED_COL(7), "    ");          // 先把后面全部清空
    oled_show_string(OLED_ROW(4), OLED_COL(1), weekday_str[Calendar.weekday - 1]);
    oled_show_int(OLED_ROW(4), OLED_COL(13), 2000 + Calendar.year, 4, false);
}
