
#include "msg_parse.h"
#include "sys_err.h"
#include <stdlib.h>

#include "system.h"
#include "bt.h"
#include "display.h"
#include "app_music.h"
#include "app_clock.h"

// #define TAG "PARSE"

#define PARSE_STATE_NORMAL 0
#define PARSE_STATE_CONFIRM_RESET 1

static u8 parse_state = 0;

#define parse_logi(fmt, ...) bt_printf(fmt "\r\n", ##__VA_ARGS__)
#define parse_logw(fmt, ...) bt_printf("\r\nW: " fmt "\r\n\r\n", ##__VA_ARGS__)
#define parse_loge(fmt, ...) bt_printf("\r\nE: " fmt "\r\n\r\n", ##__VA_ARGS__)

#define h_isInRange(h) ((u8)(h) < 100)
#define m_isInRange(m) ((u8)(m) < 60)
#define s_isInRange(s) ((u8)(s) < 60)

#define STR_ON "on"
#define STR_OFF "off"

static void s_message_parse_normal(const char *dat, u16 len);

/**
 * @brief Parsing uart received data.
 * 
 * @param dat uart receive buffer
 * @param len length of data
 */
void app_message_parse(const char *dat, u16 len)
{
    CHECK_FALSE_RET_VOID(dat && len > 0);

    if (parse_state == PARSE_STATE_NORMAL) {
        s_message_parse_normal(dat, len);
    }
    else if (parse_state == PARSE_STATE_CONFIRM_RESET)
    {
        if (begins_with_str(dat, "y") && len == 1)
        {
            parse_logw("System is resetting...");
            bt_send_all();
            bt_disconnect();
            sys_reset();
        }
        else
        {
            parse_logw("Reset is cancelled.");
        }
        parse_state = PARSE_STATE_NORMAL;
    }
}

static void s_message_parse_normal(const char *dat, u16 len)
{
    /* Music */
    if (begins_with_str(dat, "play"))
    {
        if (len == 4)
        {
            if (app_music_resume()) { parse_logi("Music resumes."); }
            else { parse_logi("Invalid operation."); }
        }
        else
        {
            u8 tmp_num;
            if (sscanf(dat, "play %hhd", &tmp_num) == 1)
            {
                u8 res = app_music_play(tmp_num);
                switch (res)
                {
                    case MUSIC_3V_Start_OK: parse_logi("Start playing music %d: [%s].",
                        tmp_num, ctrl.list[tmp_num]->name); break;
                    case MUSIC_3V_Start_ERROR_PARAM: parse_logi("Wrong parameter."); break;
                    case MUSIC_3V_Start_ERROR_NUM: parse_logi("Incorrect numbering."); break;
                    default: break;
                }
            }
            else
            {
                parse_logi("Wrong format.");
            }
        }
    }
    else if (begins_with_str(dat, "pause") && len == 5)
    {
        if (app_music_pause()) { parse_logi("Music is paused."); }
        else { parse_logi("Invalid operation."); }
    }
    else if (begins_with_str(dat, "stop") && len == 4)
    {
        app_music_stop();
        parse_logi("Music is stopped.");
    }
    #if 0
    else if (begins_with_str(dat, "pitch "))
    {
        u8 sum;
        pitch_parse(dat + 6, app_pitch, len - 6, &sum);
        if (pitch_start(sum)) { parse_logi("Pitch is read successfully."); }
        else { parse_logi("Invalid operation."); }
    }
    #endif

    /* Time */
    else if (begins_with_str(dat, "font "))
    {
        if (len == 6 && '1' <= dat[5] && dat[5] <= '4')
        {
            Time_FONT = (u8)(dat[5] - '0');
            parse_logi("Font is changed to font_%hd.", Time_FONT);
        }
        else parse_logi("Font is assumed to be numbered 1 ~ 4.");
    }
    else if (begins_with_str(dat, "set "))
    {
        if (begins_with_str(dat + 4, "t "))
        {
            int h, m, s;
            int num = sscanf(dat + 6, "%d %d %d", &h, &m, &s);
            Time_Stop(&Calendar);
            switch (num)
            {
                case 3: if (s < 60) Calendar.sec = s;   /* Without a break */
                case 2: if (m < 60) Calendar.min = m;   /* Without a break */
                case 1: if (h < 24) Calendar.hour = h; parse_logi("Set OK."); break;
                default: parse_logi("[usage] set t <hour> <min> <sec>. Ignoring redundant params."); break;
            }
            Time_Start(&Calendar);
        }
        
        else if (begins_with_str(dat + 4, "m "))
        {
            int mon;
            if (sscanf(dat + 6, "%d", &mon) == 1)
            {
                if (1 <= mon && mon <= 12)
                {
                    Calendar.month = mon;
                    parse_logi("Set OK.");
                }
                else
                {
                    parse_logi("Wrong range.");
                }
            }
            else
            {
                parse_logi("[usage] set m <month(number in 1 ~ 12)>");
            }
        }
        else if (begins_with_str(dat + 4, "d "))
        {
            int day;
            if (sscanf(dat + 6, "%d", &day) == 1)
            {
                if (1 <= day && day <= 31)
                {
                    Calendar.day = day;
                    parse_logi("Set OK.");
                }
                else
                {
                    parse_logi("Wrong range.");
                }
            }
            else
            {
                parse_logi("[usage] set d <day(number in 1 ~ 31)>");
            }
        }
        else if (begins_with_str(dat + 4, "y "))
        {
            int year;
            if (sscanf(dat + 6, "%d", &year) == 1)
            {
                if (year <= 255)
                {
                    Calendar.year = year;
                    parse_logi("Set OK.");
                }
                else
                {
                    parse_logi("Wrong range.");
                }
            }
            else
            {
                parse_logi("[usage] set y <year(number in 0 ~ 255)>");
            }
        }
        else if (begins_with_str(dat + 4, "w "))
        {
            int week;
            if (sscanf(dat + 6, "%d", &week) == 1)
            {
                if (1 <= week && week <= 7)
                {
                    Calendar.weekday = week;
                    parse_logi("Set OK.");
                }
                else
                {
                    parse_logi("Wrong range.");
                }
            }
            else
            {
                parse_logi("[usage] set w <week(number in 1 ~ 7)>");
            }
        }
        
        else
        {
            parse_logi("[usage] set <item> <params>\r\nwhere [item] could be: t(time), y(year), m(month), d(day), w(week).");
        }
    }

    /* Alarm */
    else if (begins_with_str(dat, "alarm"))
    {
        if (len == 5)
        {
            int i = 0;
            parse_logi("All alarms:");
            for (; i < 4; i++)
            {
                parse_logi(
                    "Alarm %d: %02d:%02d %c %s",
                    Alarms[i].No,
                    Alarms[i].hour,
                    Alarms[i].min,
                    Alarms[i].repeat ? 'R' : ' ',
                    Alarms[i].enable ? STR_ON : STR_OFF);
            }
        }
        else
        {
            int num, no, h, m;
            num = sscanf(dat + 5, "%d %d %d", &no, &h, &m);
            if (num == 3)
            {
                if (1 <= no && no <= 4)
                {
                    if (h < 24) Alarms[no - 1].hour = h;
                    if (m < 60) Alarms[no - 1].min = m;
                    parse_logi("Alarm %d is set to %02d:%02d.", no, h, m);
                }
                else parse_logi("Wrong range.");
            }
            else if (num == 1)
            {
                if (1 <= no && no <= 4)
                {
                    if (begins_with_str(dat + 8, "on"))
                    {
                        AlarmSetEnable(Alarms + no - 1, true);
                        parse_logi("Alarm %d is on.", no);
                    }
                    else if (begins_with_str(dat + 8, "off"))
                    {
                        AlarmSetEnable(Alarms + no - 1, false);
                        parse_logi("Alarm %d is off.", no);
                    }
                    else if (begins_with_str(dat + 8, "rp"))
                    {
                        AlarmSetRepeat(Alarms + no - 1, true);
                        parse_logi("Alarm %d is set repeated.", no);
                    }
                    else if (begins_with_str(dat + 8, "nrp"))
                    {
                        AlarmSetRepeat(Alarms + no - 1, false);
                        parse_logi("Alarm %d is set non-repeated.", no);
                    }
                    else
                    {
                        parse_logi("Wrong parameter.");
                    }
                }
                else parse_logi("Wrong range.");
            }
            else if (begins_with_str(dat + 5, " ring"))
            {
                int music_no;
                if (len == lenof_cstr("alarm ring") && Alarm_MusicNo != 0)
                {
                    parse_logi("Alarm ring is music %d: [%s].", Alarm_MusicNo, ctrl.list[Alarm_MusicNo]->name);
                }
                else if (sscanf(dat + 11, "%d", &music_no) == 1)
                {
                    if (0 < music_no && music_no <= ctrl.list_size)
                    {
                        Alarm_MusicNo = music_no;
                        parse_logi("Ring of alarm is set to music %d.", music_no);
                    }
                    else
                    {
                        parse_logi("Wrong range.");
                    }
                }
                else
                {
                    parse_logi("[usage] alarm ring <no(num)>. Where <no> is the numbering of the music.");
                }
            }
            else
            {
                parse_logi("[usage] alarm <no> <hour> <min> or alarm ring or\r\nalarm <no> <opt>, where <opt> could be \'on\',\'off\',\'rp\',\'nrp\'.");
            }
        }
    }

    /* Timer */
    else if (begins_with_str(dat, "timer"))
    {
        if (len == 5)
        {
            parse_logi("Timer: %02d:%02d:%02d, %s.", Timer.hour, Timer.min, Timer.sec, Timer.is_start ? "running" : "stopped");
        }
        else if (begins_with_str(dat + 5, " start"))
        {
            if (Timer.hour == 0 && Timer.min == 0 && Timer.sec == 0)
            {
                parse_logi("Invalid Operation.");
            }
            else
            {
                Timer_Cmd(&Timer, ENABLE);
                parse_logi("Timer is started.");
            }
        }
        else if (begins_with_str(dat + 5, " pause"))
        {
            Timer_Cmd(&Timer, DISABLE);
            parse_logi("Timer is paused.");
        }
        else if (begins_with_str(dat + 5, " clear"))
        {
            Timer_Cmd(&Timer, DISABLE);
            Timer_Clear(&Timer);
            parse_logi("Timer is stopped and cleared.");
        }
        else if (begins_with_str(dat + 5, " ring "))
        {
            int no;
            if (len == lenof_cstr("timer ring") && Timer_MusicNo != 0)
            {
                parse_logi("Timer ring is music %d: [%s].", Timer_MusicNo, ctrl.list[Timer_MusicNo]->name);
            }
            else if (sscanf(dat + 11, "%d", &no) == 1)
            {
                if (0 < no && no <= ctrl.list_size)
                {
                    Timer_MusicNo = no;
                    parse_logi("Timer ring is set to music %d.", no);
                }
                else
                {
                    parse_logi("Wrong range.");
                }
            }
            else
            {
                parse_logi("[usage] timer ring <no(num)>. Where no is the numbering of the music.");
            }
        }
        else
        {
            int in1, in2, in3;
            int num = sscanf(dat + 5, " %d %d %d", &in1, &in2, &in3);

            if (num >= 1 && num <= 3)
            {
                if (Timer.is_start)
                {
                    parse_logi("Please clear or pause the timer first.");
                }
                else
                {
                    switch (num)
                    {
                        case 1:
                        {
                            if (s_isInRange(in1))
                            {
                                Timer.sec = in1;
                                Timer.min = 0;
                                Timer.hour = 0;
                                parse_logi("Timer is set %02d:%02d:%02d.", Timer.hour, Timer.min, Timer.sec);
                            }
                            else
                            {
                                parse_logi("Wrong range.");
                            }
                        } break;
                        
                        case 2:
                        {
                            if (m_isInRange(in1) && s_isInRange(in2))
                            {
                                Timer.sec = in2;
                                Timer.min = in1;
                                Timer.hour = 0;
                                parse_logi("Timer is set %02d:%02d:%02d.", Timer.hour, Timer.min, Timer.sec);
                            }
                            else
                            {
                                parse_logi("Wrong range.");
                            }
                        } break;
                        
                        case 3:
                        {
                            if (s_isInRange(in3) && m_isInRange(in2) && h_isInRange(in1))
                            {
                                Timer.sec = in3;
                                Timer.min = in2;
                                Timer.hour = in1;
                                parse_logi("Timer is set %02d:%02d:%02d.", Timer.hour, Timer.min, Timer.sec);
                            }
                            else
                            {
                                parse_logi("Wrong range.");
                            }
                        } break;

                        default: break;
                    }
                }
            }
            else
            {
                parse_logi("[usage] timer <opt>. Where <opt> could be \'start\',\'pause\',\'clear\',\'ring\'. Or timer <hour> <min> <sec> to set time.");
            }
        }
    }

    /* Watch */
    else if (begins_with_str(dat, "watch"))
    {
        if (len == 5)
        {
            parse_logi("Watch: %02d:%02d:%02d, %s.", Watch.hour, Watch.min, Watch.sec, Watch.is_start ? "running" : "stopped");
        }
        else if (begins_with_str(dat + 5, " start"))
        {
            Watch_Cmd(&Watch, ENABLE);
            parse_logi("Watch is started.");
        }
        else if (begins_with_str(dat + 5, " pause"))
        {
            Watch_Cmd(&Watch, DISABLE);
            parse_logi("Watch is paused.");
        }
        else if (begins_with_str(dat + 5, " clear"))
        {
            Watch_Cmd(&Watch, DISABLE);
            Watch_Clear(&Watch);
            parse_logi("Watch is stopped and cleared.");
        }
        else
        {
            parse_logi("[usage] watch <opt>. Where <opt> could be \'start\',\'pause\',\'clear\'.");
        }
    }

    /* Pages */
    else if (begins_with_str(dat, "page "))
    {
        bool cmd_valid = true;
        if (begins_with_str(dat + 5, "main")) curr_page = PAGE_Main;
        else if (begins_with_str(dat + 5, "cldr")) curr_page = PAGE_Cldr;
        else if (begins_with_str(dat + 5, "alarm")) curr_page = PAGE_Alarm;
        else if (begins_with_str(dat + 5, "timer")) curr_page = PAGE_Timer;
        else if (begins_with_str(dat + 5, "watch")) curr_page = PAGE_Watch;
        else
        {
            cmd_valid = false;
            parse_logi("[usage] page <name>. Where <name> could be \'main\',\'cldr\',\'alarm\',\'timer\',\'watch\'.");
        }
        if (cmd_valid) display_subtrate();
    }

    #if 0
    /* Swing */
    else if (begins_with_str(dat, "swing "))
    {
        if (begins_with_str(dat + 6, "on"))
        {
            app_servo_swingcmd(&cs1, ENABLE);
            parse_logi("Swing is ""on.");
        }
        else if (begins_with_str(dat + 6, "off"))
        {
            app_servo_swingcmd(&cs1, DISABLE);
            parse_logi("Swing is ""off.");
        }
        else if (begins_with_str(dat + 6, "a "))
        {
            int angtmp;
            if (sscanf(dat + 8, "%d", &angtmp) == 1)
            {
                if (app_servo_setswangle(&cs1, (float)angtmp)) parse_logi("Set OK.");
                else parse_logi("Wrong range.");
            }
            else
            {
                parse_logi("[usage] swing sw <angle(int)>. Where <angle> could be an angle at 0 ~ 90.");
            }
        }
        else if (begins_with_str(dat + 6, "rest "))
        {
            int angtmp;
            if (sscanf(dat + 11, "%d", &angtmp) == 1)
            {
                app_servo_setrestangle(&cs1, (float)angtmp);
                parse_logi("Set OK.");
            }
            else
            {
                parse_logi("[usage] swing rest <angle(int)>. Where <angle> could be an angle at 0 ~ 180.");
            }
        }
        else if (begins_with_str(dat + 6, "alm "))
        {
            if (begins_with_str(dat + 6 + 4, "on"))
            {
                swing_along_music = true;
                parse_logi("Set OK.");
            }
            else if (begins_with_str(dat + 6 + 4, "off"))
            {
                swing_along_music = false;
                parse_logi("Set OK.");
            }
            else
            {
                parse_logi("[usage] swing alm <on/off>. decide if swings along music beats.");
            }
        }
        else if (begins_with_str(dat + 6, "st "))
        {
            if (begins_with_str(dat + 9, "on"))
            {
                app_servo_setstill(&cs1, ENABLE);
                parse_logi("Set OK.");
            }
            else if (begins_with_str(dat + 9, "off"))
            {
                app_servo_setstill(&cs1, DISABLE);
                parse_logi("Set OK.");
            }
            else
            {
                parse_logi("[usage] swing st <on/off>. Sets if the swing maintains its angle when still.");
            }
        }
        else
        {
            parse_logi("[usage] swing <...>. Where <...> could be \'on\',\'off\',\'a\',\'rest\',\'alm\',\'st\'.");
        }
    }
    #endif

    /* Time */
    else if (begins_with_str(dat, "time") && len == 4)
    {
        parse_logi("Current time: %02d:%02d:%02d.", (int)(Calendar.hour), (int)(Calendar.min), (int)(Calendar.sec));
    }
    else if (begins_with_str(dat, "date") && len == 4)
    {
        parse_logi(
            "Date: %4d.%d.%d, %s.",
            (int)(2000 + Calendar.year),
            (int)(Calendar.month),
            (int)(Calendar.day),
            weekday_str[Calendar.weekday - 1]
        );
    }

    /* Screen */
    else if (begins_with_str(dat, "scr"))
    {
        if (begins_with_str(dat + 3, " on"))
        {
            display_scr_on_off(true);
            parse_logi("Screen is on.");
        }
        else if (begins_with_str(dat + 3, " off"))
        {
            display_scr_on_off(false);
            parse_logi("Screen is off.");
        }
        else if (begins_with_str(dat + 3, " reinit"))
        {
            parse_logi("Start reinit.");
            display_init();
            parse_logi("Reinit OK.");
        }
        else
        {
            parse_logi("[usage] (1) scr <on/off>; (2) scr debug <on/off>; (3) scr reinit.");
        }
    }

    /* Bluetooth */
    else if (begins_with_str(dat, "bt-baud")) {
        u32 baud = atoi((char*)dat + lenof_cstr("bt-baud") + 1);
        bt_change_baud(baud);
    }
    else if (begins_with_str(dat, "bt-reinit")) {
        parse_logi("Starting BLE reinit...");
        bt_init();
    }

    /* Help */
    else if (begins_with_str(dat, "hello") || begins_with_str(dat, "help"))
    {
        bt_send_all();
        parse_logi( "Hello! This is your lovely clock. Here are the commands:\r\n"
                    "\t(1) Music:\r\n\t\t(a) play [<num>] (b) stop (c) pause\r\n"
                    "\t(2) Pitch: pitch [<pitches>]\r\n");
        bt_send_all();
        LL_mDelay(20);
        parse_logi( "\t(3) Pages: page <name>\r\n"
                    "\t(4) Time:\r\n\t\t(a) font <num> (b) set <...> (c) time (d) date\r\n"
                    "\t(5) Alarm: alarm <...>\r\n"
                    "\t(6) Timer: timer <...>\r\n");
        bt_send_all();
        LL_mDelay(20);
        parse_logi( "\t(7) Watch: watch <...>\r\n"
                    "\t(8) Screen: (a) scr reinit (b) scr <on/off>\r\n"
                    "Type them with a space to see their instructions.");
    }

    /* Greetings */
    else if (begins_with_str(dat, "我要视奸你") ||
             begins_with_str(dat, "沙壁") ||
             begins_with_str(dat, "傻波一") ||
             begins_with_str(dat, "啥比") ||
             begins_with_str(dat, "啥必") ||
             begins_with_str(dat, "煞笔") ||
             begins_with_str(dat, "你个必养的"))
    {
        parse_logi("我噪称冯了个福，你tm就是个急把");
    }

    /* Debug */
    else if (begins_with_str(dat, "sys reset") && len == 9)
    {
        parse_logw("Are you sure to reset the system? (Type \"y\" to perform or other to cancel)");
        parse_state = PARSE_STATE_CONFIRM_RESET;
    }

    /* No cmd  */
    else
    {
        parse_logw("Command does not exist.");
    }

    // parse_logi("Recv: %.*s, len = %hd", len, dat, len);
}
