#ifndef __TIME_H
#define __TIME_H

#include "main.h"

typedef struct
{
    u8 sec;
    u8 min;
    u8 hour;
    u8 weekday; // range: 1-7, 1 for Monday, 7 for Sunday
    u8 day;
    u8 month;
    u8 year;

    bool run;
} Calendar_t;

/**
 *  @note Don't modify these members by yourself while you're assumed to
 *        call the functions, except initializatioin.
 * 
 */
typedef struct
{
    u8 No;
    u8 min;
    u8 hour;
    bool enable;
    bool alarming;
    bool repeat;
} Alarm_t;

typedef struct
{
    u8 hour;
    u8 min;
    u8 sec;

    bool is_start;
} Timer_t;

typedef void(*Alarm_CB_t)(void);
typedef void(*Timer_CB_t)(void);


void Time_Inc1s_Handler(Calendar_t *cldr);
void Time_Start(Calendar_t *cldr);
void Time_Stop(Calendar_t *cldr);

void Alarm_Check(Calendar_t *cldr, Alarm_t *alarm);
void AlarmSetEnable(Alarm_t *alarm, bool if_enable);
void AlarmSetRepeat(Alarm_t *alarm, bool if_repeat);
void AlarmSetTime(Alarm_t *alarm, u8 hour, u8 min);
void AlarmSetCB(Alarm_CB_t cb);

void Timer_Clear(Timer_t *timer);
void Timer_Cmd(Timer_t *timer, bool cmd);
void Timer_Check(Timer_t *timer);
void Timer_SetCB(Timer_CB_t cb);

void Watch_Clear(Timer_t *watch);
void Watch_Run(Timer_t *watch);
void Watch_Cmd(Timer_t *watch, bool cmd);

#endif // !__TIME_H
