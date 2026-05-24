
#include "app_clock.h"
#include "app_music.h"
#include "debug.h"
#include "bt.h"

#define TAG "Time"

#define IconExist_TIME                  (60)

/*-------------------------------- Variables -------------------------------*/

static void Alarm_Handler(void);
static void Timer_Handler(void);
static void Timer_IconHandler(void);

Timer_t Timer;
Timer_t Watch;

u8 Timer_IconExistCnt = false, Alarm_IconExistCnt = false;
u8 Timer_MusicNo = 3, Alarm_MusicNo = 2;


Calendar_t Calendar =
{
    .sec = 50,
    .min = 59,
    .hour = 23,
    .weekday = 1,
    .day = 11,
    .month = 5,
    .year = 26,
    .run = true
};

Alarm_t Alarms[4] =
{
    {
        .No = 1,
        .hour = 6,
        .min = 30,
    },
    {
        .No = 2,
        .hour = 10,
        .min = 00,
    },
    {
        .No = 3,
        .hour = 13,
        .min = 30,
    },
    {
        .No = 4,
        .hour = 21,
        .min = 00,
    }
};

const char *weekday_str[7] =
{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};


/*-------------------------------- Functions -------------------------------*/

void Clock_Init(void)
{
    AlarmSetCB(Alarm_Handler);
    Timer_SetCB(Timer_Handler);

    Timer_Clear(&Timer);
    Timer_Cmd(&Timer, DISABLE);
    Watch_Clear(&Watch);
    Watch_Cmd(&Watch, DISABLE);

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_EnableIT_SEC(RTC);
    LL_RTC_EnableWriteProtection(RTC);
}

void Clock_Inc1s_Handler(void)
{
    if (LL_RTC_IsActiveFlag_SEC(RTC)) {
        LL_RTC_ClearFlag_SEC(RTC);
        
        Time_Inc1s_Handler(&Calendar);
        Alarm_Check(&Calendar, &Alarms[0]);
        Alarm_Check(&Calendar, &Alarms[1]);
        Alarm_Check(&Calendar, &Alarms[2]);
        Alarm_Check(&Calendar, &Alarms[3]);
        Timer_Check(&Timer);
        Watch_Run(&Watch);
        Timer_IconHandler();
    }
}

static void Alarm_Handler(void)
{
    app_music_play(Alarm_MusicNo);
    Alarm_IconExistCnt = IconExist_TIME + 1;
    bt_printf(TAG, "Alarm: alarm time reached.\r\n");
}


static void Timer_Handler(void)
{
    app_music_play(Timer_MusicNo);
    Timer_IconExistCnt = IconExist_TIME + 1;
    bt_printf(TAG, "Timer: time's up.\r\n");
}


/**
 * @brief When timer time or alarm reaches, keep correspondent icon
 *        exist for a certain time.
 * 
 */
static void Timer_IconHandler(void)
{
    Timer_IconExistCnt > 0 ? --Timer_IconExistCnt : (void)0;
    Alarm_IconExistCnt > 0 ? --Alarm_IconExistCnt : (void)0;
}
