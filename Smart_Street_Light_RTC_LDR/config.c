#include "types.h"
#include "config.h"

static u8 g_mode = MODE_AUTO;

/*
   Default schedule:
   ON  = 18:00
   OFF = 06:00
*/
static u8 g_on_hour  = 18;
static u8 g_on_min   = 0;
static u8 g_off_hour = 6;
static u8 g_off_min  = 0;

static u32 g_last_adc = 0;
static u8 g_brightness = 0;


static u32 g_threshold_100 = 300;
static u32 g_threshold_60  = 600;
static u32 g_threshold_30  = 850;



void config_init(void)
{
    g_mode = MODE_AUTO;

    g_on_hour  = 18;
    g_on_min   = 0;
    g_off_hour = 6;
    g_off_min  = 0;

    g_last_adc = 0;
    g_brightness = 0;
	
	  g_threshold_100 = 300;
    g_threshold_60  = 600;
    g_threshold_30  = 850;
}

void config_set_mode(u8 mode)
{
    if(mode <= MODE_MANUAL_OFF)
    {
        g_mode = mode;
    }
}

u8 config_get_mode(void)
{
    return g_mode;
}

char *config_get_mode_text(void)
{
    switch(g_mode)
    {
        case MODE_AUTO:
            return "AUTO";

        case MODE_MANUAL_ON:
            return "MANON";

        case MODE_MANUAL_OFF:
            return "MANOF";

        default:
            return "ERR";
    }
}

void config_set_on_time(u8 hour, u8 min)
{
    if(hour <= 23 && min <= 59)
    {
        g_on_hour = hour;
        g_on_min = min;
    }
}

void config_set_off_time(u8 hour, u8 min)
{
    if(hour <= 23 && min <= 59)
    {
        g_off_hour = hour;
        g_off_min = min;
    }
}

void config_get_on_time(u8 *hour, u8 *min)
{
    *hour = g_on_hour;
    *min = g_on_min;
}

void config_get_off_time(u8 *hour, u8 *min)
{
    *hour = g_off_hour;
    *min = g_off_min;
}

/*
   Handles normal and overnight schedules.

   Example:
   ON  = 18:00
   OFF = 06:00

   Active:
   18:00 to 23:59
   00:00 to 05:59
*/
u8 config_is_light_schedule_active(u8 hour, u8 min)
{
    u32 now_mins;
    u32 on_mins;
    u32 off_mins;

    now_mins = (hour * 60) + min;
    on_mins = (g_on_hour * 60) + g_on_min;
    off_mins = (g_off_hour * 60) + g_off_min;

    if(on_mins < off_mins)
    {
        if(now_mins >= on_mins && now_mins < off_mins)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if(now_mins >= on_mins || now_mins < off_mins)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

void config_set_last_adc(u32 adc)
{
    g_last_adc = adc;
}

u32 config_get_last_adc(void)
{
    return g_last_adc;
}

void config_set_brightness(u8 brightness)
{
    g_brightness = brightness;
}

u8 config_get_brightness(void)
{
    return g_brightness;
}



void config_set_ldr_thresholds(u32 threshold_100, u32 threshold_60, u32 threshold_30)
{
    /*
       For your present LDR connection:
       +3.3V -> LDR -> ADC -> 10k -> GND

       Darkness gives LOW ADC.
       So order should be:
       threshold_100 < threshold_60 < threshold_30
    */
    if((threshold_100 < threshold_60) &&
       (threshold_60 < threshold_30) &&
       (threshold_30 <= 1023))
    {
        g_threshold_100 = threshold_100;
        g_threshold_60  = threshold_60;
        g_threshold_30  = threshold_30;
    }
}

u32 config_get_threshold_100(void)
{
    return g_threshold_100;
}

u32 config_get_threshold_60(void)
{
    return g_threshold_60;
}

u32 config_get_threshold_30(void)
{
    return g_threshold_30;
}


