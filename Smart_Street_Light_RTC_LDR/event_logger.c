#include "types.h"
#include "event_logger.h"
#include "rtc.h"
#include "lcd.h"
#include "timer.h"

#define EVENT_LOG_SIZE   8

typedef struct
{
    u8 event_code;
    u8 hour;
    u8 min;
    u8 sec;
} EVENT_RECORD;

static EVENT_RECORD g_event_log[EVENT_LOG_SIZE];
static u8 g_event_count = 0;
static u8 g_event_write_index = 0;

static void lcd_print_2digit_event(u32 value)
{
    charlcd((value / 10) + '0');
    charlcd((value % 10) + '0');
}

void event_logger_init(void)
{
    u8 i;

    g_event_count = 0;
    g_event_write_index = 0;

    for(i = 0; i < EVENT_LOG_SIZE; i++)
    {
        g_event_log[i].event_code = 0;
        g_event_log[i].hour = 0;
        g_event_log[i].min = 0;
        g_event_log[i].sec = 0;
    }
}

char *event_get_text(u8 event_code)
{
    switch(event_code)
    {
        case EVENT_SYSTEM_START:
            return "SYSTEM START";

        case EVENT_MODE_AUTO:
            return "MODE AUTO";

        case EVENT_MODE_MANUAL_ON:
            return "MANUAL ON";

        case EVENT_MODE_MANUAL_OFF:
            return "MANUAL OFF";

        case EVENT_RTC_UPDATED:
            return "RTC UPDATED";

        case EVENT_ON_TIME_UPDATED:
            return "ON TIME SAVED";

        case EVENT_OFF_TIME_UPDATED:
            return "OFF TIME SAVED";

        case EVENT_CAL_UPDATED:
            return "CAL UPDATED";

        case EVENT_BRIGHTNESS_CHANGED:
            return "BRIGHTNESS CHG";

        case EVENT_MOTION_DETECTED:
            return "MOTION DETECT";

        case EVENT_ALARM_ON:
            return "ON ALARM";

        case EVENT_ALARM_OFF:
            return "OFF ALARM";

        case EVENT_FAULT_DETECTED:
            return "FAULT DETECT";

        default:
            return "UNKNOWN";
    }
}

void event_log(u8 event_code)
{
    s32 hour, min, sec;

    GetRTCTimeInfo(&hour, &min, &sec);

    g_event_log[g_event_write_index].event_code = event_code;
    g_event_log[g_event_write_index].hour = (u8)hour;
    g_event_log[g_event_write_index].min = (u8)min;
    g_event_log[g_event_write_index].sec = (u8)sec;

    g_event_write_index++;

    if(g_event_write_index >= EVENT_LOG_SIZE)
    {
        g_event_write_index = 0;
    }

    if(g_event_count < EVENT_LOG_SIZE)
    {
        g_event_count++;
    }
}

void event_logger_show_history(void)
{
    u8 i;
    u8 index;

    if(g_event_count == 0)
    {
        lcd_clear();

        lcd_gotoxy(0, 0);
        strlcd("NO EVENTS");

        lcd_gotoxy(1, 0);
        strlcd("LOG EMPTY");

        tdelay_ms(1500);
        lcd_clear();
        return;
    }

    for(i = 0; i < g_event_count; i++)
    {
        if(g_event_write_index == 0)
        {
            index = EVENT_LOG_SIZE - 1;
        }
        else
        {
            index = g_event_write_index - 1;
        }

        if(index >= i)
        {
            index = index - i;
        }
        else
        {
            index = EVENT_LOG_SIZE + index - i;
        }

        lcd_clear();

        lcd_gotoxy(0, 0);
        lcd_print_2digit_event(g_event_log[index].hour);
        charlcd(':');
        lcd_print_2digit_event(g_event_log[index].min);
        charlcd(':');
        lcd_print_2digit_event(g_event_log[index].sec);

        lcd_gotoxy(1, 0);
        strlcd(event_get_text(g_event_log[index].event_code));

        tdelay_ms(1800);
    }

    lcd_clear();
}

