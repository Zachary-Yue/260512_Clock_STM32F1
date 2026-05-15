
#pragma once

#include "main.h"
#include "clock.h"

extern Calendar_t Calendar;
extern Alarm_t Alarms[4];
extern Timer_t Timer;
extern Timer_t Watch;

extern u8 Timer_IconExistCnt, Alarm_IconExistCnt;
extern u8 Timer_MusicNo, Alarm_MusicNo;
extern const char *weekday_str[7];

void Clock_Init(void);
void Clock_Inc1s_Handler(void);
