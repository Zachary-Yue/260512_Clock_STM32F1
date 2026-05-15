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
#include "Page_Example.h"


void Page_Example_Subtrate(void)
{
    oled_show_cn16_str(OLED_ROW(1), OLED_COL(1), "Example");
}

void Page_Example_Task(void)
{
    ;
}
