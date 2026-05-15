/**
 * @file Page_Timer.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 页面编写示例。使用时可以在编辑器里将 Timer 字样统一替换成自己页面的名字。
 * 
 * @version 0.1
 * @date 2025-07-21
 * 
 */
#include "Page_Timer.h"
#include "app_clock.h"


void Page_Timer_Subtrate(void)
{
    // OLED_ShowCnString16(1, 1, "计时器");
    oled_show_cn16_str(OLED_ROW(1), OLED_COL(1), "计时器");
    oled_show_char(OLED_ROW(1), OLED_COL(14), ':');
    
    oled_show_num3216(OLED_ROW(3), OLED_COL(5), 0, FONT_COLON3216);
    oled_show_num3216(OLED_ROW(3), OLED_COL(11), 0, FONT_COLON3216);
}

void Page_Timer_Task(void)
{
    oled_show_int(OLED_ROW(1), OLED_COL(12), Calendar.hour, 2, true);
    oled_show_int(OLED_ROW(1), OLED_COL(15), Calendar.min, 2, true);

    oled_show_num3216(OLED_ROW(3), OLED_COL(1), Timer.hour / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(3), Timer.hour % 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(7), Timer.min / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(9), Timer.min % 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(13), Timer.sec / 10, FONT_NUM3216_1);
    oled_show_num3216(OLED_ROW(3), OLED_COL(15), Timer.sec % 10, FONT_NUM3216_1);
}
