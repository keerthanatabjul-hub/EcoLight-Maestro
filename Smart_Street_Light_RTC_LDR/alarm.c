#include <lpc21xx.h>
#include "types.h"
#include "alarm.h"
#include "rtc.h"
#include "config.h"
#include "lcd.h"

#define BUZZER_PIN          11
#define ALARM_DURATION_MS    3000   /* 30 x 100ms = 3 seconds */

static u8 alarm_active = 0;
static u32 alarm_time_ms = 0;
static u8 buzzer_toggle_state = 0;

static u8 last_on_alarm_hour = 255;
static u8 last_on_alarm_min  = 255;

static u8 last_off_alarm_hour = 255;
static u8 last_off_alarm_min  = 255;

static void buzzer_on(void)
{
    IOSET0 = (1 << BUZZER_PIN);
}

static void buzzer_off(void)
{
    IOCLR0 = (1 << BUZZER_PIN);
}

void alarm_init(void)
{
	/*
       Configure P0.11 as GPIO.
       P0.11 uses PINSEL0 bits 23:22.
    */
    PINSEL0 &= ~(3 << 22);
    IODIR0 |= (1 << BUZZER_PIN);

    buzzer_off();

    alarm_active = 0;
    alarm_time_ms = 0;
    buzzer_toggle_state = 0;
}

u8 alarm_is_active(void)
{
    return alarm_active;
}

static void alarm_start(char *line1, char *line2)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd(line1);

    lcd_gotoxy(1, 0);
    strlcd(line2);

    alarm_active = 1;
	  alarm_time_ms = ALARM_DURATION_MS;
    buzzer_toggle_state = 0;
    
}

void alarm_check_task(void)
{
    s32 hour, min, sec;
    u8 on_h, on_m;
    u8 off_h, off_m;

    GetRTCTimeInfo(&hour, &min, &sec);

    config_get_on_time(&on_h, &on_m);
    config_get_off_time(&off_h, &off_m);

    /*
       Trigger ON alarm only once when HH:MM:00 matches ON time.
    */
    if((hour == on_h) && (min == on_m) && (sec == 0))
    {
        if(last_on_alarm_hour != on_h || last_on_alarm_min != on_m)
        {
            alarm_start("ON TIME ALARM", "LIGHT ACTIVE");

            last_on_alarm_hour = on_h;
            last_on_alarm_min = on_m;
        }
    }

    /*
       Reset ON alarm memory after minute changes.
    */
    if(min != on_m)
    {
        last_on_alarm_hour = 255;
        last_on_alarm_min = 255;
    }

    /*
       Trigger OFF alarm only once when HH:MM:00 matches OFF time.
    */
    if((hour == off_h) && (min == off_m) && (sec == 0))
    {
        if(last_off_alarm_hour != off_h || last_off_alarm_min != off_m)
        {
            alarm_start("OFF TIME ALARM", "LIGHT INACTIVE");

            last_off_alarm_hour = off_h;
            last_off_alarm_min = off_m;
        }
    }

    /*
       Reset OFF alarm memory after minute changes.
    */
    if(min != off_m)
    {
        last_off_alarm_hour = 255;
        last_off_alarm_min = 255;
    }
}

void alarm_buzzer_task_1ms(void)
{
    if(alarm_active == 0)
    {
        buzzer_off();
        return;
    }

    if(alarm_time_ms > 0)
    {
        alarm_time_ms--;

        /*
           Toggle every 1 ms.
           This gives around 500 Hz sound.
        */
        buzzer_toggle_state = !buzzer_toggle_state;

        if(buzzer_toggle_state)
        {
            buzzer_on();
        }
        else
        {
            buzzer_off();
        }
    }
    else
    {
        buzzer_off();
        alarm_active = 0;
        lcd_clear();
    }
}

