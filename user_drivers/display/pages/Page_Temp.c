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

void Page_Temp_Subtrate(void)
{
    oled_show_string(OLED_ROW(1), OLED_COL(1), "Temperature");
    oled_show_icon16(OLED_ROW(3), OLED_COL(5), Icon16_TEMPERATURE);
    oled_show_string(OLED_ROW(3), OLED_COL(7), "  . `C");
}

void Page_Temp_Task(void)
{
    oled_show_int(OLED_ROW(3), OLED_COL(7), t_int, 2, false);
    oled_show_int(OLED_ROW(3), OLED_COL(10), t_deci_1, 1, true);
}
