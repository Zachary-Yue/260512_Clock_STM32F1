/**
 * @file Page_Alarm.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 页面编写示例。使用时可以在编辑器里将 Alarm 字样统一替换成自己页面的名字。
 * 
 * @version 0.1
 * @date 2025-07-21
 * 
 */
#include "Page_Alarm.h"
#include "app_clock.h"


void Page_Alarm_Subtrate(void)
{
    oled_show_cn16_str(OLED_ROW(2), OLED_COL(1), "闹");
    oled_show_cn16_str(OLED_ROW(3), OLED_COL(1), "钟");

    oled_show_char(OLED_ROW(1), OLED_COL(4), '1');
    oled_show_char(OLED_ROW(2), OLED_COL(4), '2');
    oled_show_char(OLED_ROW(3), OLED_COL(4), '3');
    oled_show_char(OLED_ROW(4), OLED_COL(4), '4');

    oled_show_char(OLED_ROW(1), OLED_COL(9), ':');
    oled_show_char(OLED_ROW(2), OLED_COL(9), ':');
    oled_show_char(OLED_ROW(3), OLED_COL(9), ':');
    oled_show_char(OLED_ROW(4), OLED_COL(9), ':');
}

void Page_Alarm_Task(void)
{
    oled_show_int(OLED_ROW(1), OLED_COL(7), Alarms[0].hour, 2, true);
    oled_show_int(OLED_ROW(2), OLED_COL(7), Alarms[1].hour, 2, true);
    oled_show_int(OLED_ROW(3), OLED_COL(7), Alarms[2].hour, 2, true);
    oled_show_int(OLED_ROW(4), OLED_COL(7), Alarms[3].hour, 2, true);

    oled_show_int(OLED_ROW(1), OLED_COL(10), Alarms[0].min, 2, true);
    oled_show_int(OLED_ROW(2), OLED_COL(10), Alarms[1].min, 2, true);
    oled_show_int(OLED_ROW(3), OLED_COL(10), Alarms[2].min, 2, true);
    oled_show_int(OLED_ROW(4), OLED_COL(10), Alarms[3].min, 2, true);

    oled_show_char(OLED_ROW(1), OLED_COL(13), (Alarms[0].repeat ? 'R' : ' '));
    oled_show_char(OLED_ROW(2), OLED_COL(13), (Alarms[1].repeat ? 'R' : ' '));
    oled_show_char(OLED_ROW(3), OLED_COL(13), (Alarms[2].repeat ? 'R' : ' '));
    oled_show_char(OLED_ROW(4), OLED_COL(13), (Alarms[3].repeat ? 'R' : ' '));

    oled_show_icon16(OLED_ROW(1), OLED_COL(15),
        (Alarms[0].enable ? Icon16_CHECK_MARK : Icon16_EMPTY_SQUARE));
    oled_show_icon16(OLED_ROW(2), OLED_COL(15),
        (Alarms[1].enable ? Icon16_CHECK_MARK : Icon16_EMPTY_SQUARE));
    oled_show_icon16(OLED_ROW(3), OLED_COL(15),
        (Alarms[2].enable ? Icon16_CHECK_MARK : Icon16_EMPTY_SQUARE));
    oled_show_icon16(OLED_ROW(4), OLED_COL(15),
        (Alarms[3].enable ? Icon16_CHECK_MARK : Icon16_EMPTY_SQUARE));
}
