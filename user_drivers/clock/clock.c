
#include "clock.h"

static Alarm_CB_t alarm_cb = NULL;

static bool IsLeapYear(u8 year)
{
    if (year % 400 == 0) return true;
    else if (year % 100 == 0) return false;
    else if ((year & 0x3) == 0) return true;
    return false;
}

static void Time_IncMonth(Calendar_t *cldr)
{
    if (++cldr->month > 12)
    {
        cldr->month = 1;
        ++cldr->year;
    }
}

static void Time_IncBy1s(Calendar_t *cldr)
{
    Calendar_t temp = *cldr;

    if (++temp.sec >= 60)
    {
        temp.sec = 0;
        if (++temp.min >= 60)
        {
            temp.min = 0;
            if (++temp.hour >= 24)
            {
                temp.hour = 0;
                temp.day++;
                (++temp.weekday > 7) ? (temp.weekday = 1) : (void)0;
                switch (temp.month)
                {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                    case 12:
                    {
                        if (temp.day > 31)
                        {
                            temp.day = 1;
                            Time_IncMonth(&temp);
                        }
                    } break;

                    case 4:
                    case 6:
                    case 9:
                    case 11:
                    {
                        if (temp.day > 30)
                        {
                            temp.day = 1;
                            Time_IncMonth(&temp);
                        }
                    } break;

                    case 2:
                    {
                        if (IsLeapYear(temp.year))
                        {
                            if (temp.day > 29)
                            {
                                temp.day = 1;
                                Time_IncMonth(&temp);
                            }
                        }
                        else
                        {
                            if (temp.day > 28)
                            {
                                temp.day = 1;
                                Time_IncMonth(&temp);
                            }
                        }
                    } break;
                }
            }
        }
    }

    *cldr = temp;
}


void Time_Inc1s_Handler(Calendar_t *cldr)
{
    if (cldr->run == false) return;
    Time_IncBy1s(cldr);
}

void Time_Stop(Calendar_t *cldr)
{
    cldr->run = false;
}

void Time_Start(Calendar_t *cldr)
{
    cldr->run = true;
}


#define time_to_alarm(_pcldr, _palarm)      ((_pcldr)->hour == (_palarm)->hour && (_pcldr)->min == (_palarm)->min)
#define alarm_start(_palarm)                ((_palarm)->alarming = true)
#define alarm_stop(_palarm)                 ((_palarm)->alarming = false)
#define alarm_enable(_palarm)               ((_palarm)->enable = true)
#define alarm_disable(_palarm)              ((_palarm)->enable = false)
#define is_alarm_enabled(_palarm)           ((_palarm)->enable == true)
#define is_alarming(_palarm)                ((_palarm)->alarming == true)
#define is_alarm_NOT_repeat(_palarm)        ((_palarm)->repeat == false)

void Alarm_Check(Calendar_t *cldr, Alarm_t *alarm)
{
    if (is_alarming(alarm))
    {
        if (!time_to_alarm(cldr, alarm)) alarm_stop(alarm);
    }
    else
    {
        if (is_alarm_enabled(alarm))
        {
            if (time_to_alarm(cldr, alarm))
            {
                alarm_start(alarm);
                if (is_alarm_NOT_repeat(alarm)) alarm_disable(alarm);
                if (alarm_cb != NULL) alarm_cb();
            }
        }
    }
}

void AlarmSetEnable(Alarm_t *alarm, bool if_enable)
{
    if (if_enable)
    {
        alarm_enable(alarm);
        alarm_start(alarm);
    }
    else
    {
        alarm_disable(alarm);
    }
}

void AlarmSetRepeat(Alarm_t *alarm, bool if_repeat)
{
    alarm->repeat = if_repeat;
}

void AlarmSetCB(Alarm_CB_t cb)
{
    alarm_cb = cb;
}

/**
 * @brief Set the Alarm Time object
 * @note  Automatically disable the alarm when setting it, and enable when finish setting.
 * 
 */
void AlarmSetTime(Alarm_t *alarm, u8 hour, u8 min)
{
    AlarmSetEnable(alarm, true);
    alarm->hour = hour, alarm->min = min;
    AlarmSetEnable(alarm, false);
}


static Timer_CB_t timer_cb = NULL;

void Timer_Clear(Timer_t *timer)
{
    timer->hour = 0;
    timer->min = 0;
    timer->sec = 0;
}

void Timer_Cmd(Timer_t *timer, bool cmd)
{
    timer->is_start = cmd;
}

void Timer_Check(Timer_t *timer)
{
    if (timer->is_start == false) return;

    if (timer->sec == 0)
    {
        if (timer->min == 0)
        {
            if (timer->hour == 0)
            {
                timer->is_start = false;
                if (timer_cb != NULL) timer_cb();
            }
            else
            {
                --timer->hour;
                timer->min = 59;
                timer->sec = 59;
            }
        }
        else
        {
            --timer->min;
            timer->sec = 59;
        }
    }
    else --timer->sec;
}

void Timer_SetCB(Timer_CB_t cb)
{
    timer_cb = cb;
}


void Watch_Clear(Timer_t *watch)
{
    watch->hour = 0;
    watch->min = 0;
    watch->sec = 0;
}

void Watch_Cmd(Timer_t *watch, bool cmd)
{
    watch->is_start = cmd;
}

void Watch_Run(Timer_t *watch)
{
    if (watch->is_start == false) return;
    
    if (++watch->sec >= 60)
    {
        watch->sec = 0;
        if (++watch->min >= 60)
        {
            watch->min = 0;
            if (++watch->hour > 99)
            {
                watch->hour = 0;
                Watch_Cmd(watch, DISABLE);
            }
        }
    }
}
