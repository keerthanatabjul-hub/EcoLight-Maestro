#include "types.h"
#include "fault.h"
#include "config.h"
#include "rtc.h"
#include "date_utils.h"

static u8 g_fault_code = FAULT_NONE;

void fault_check(void)
{
    s32 hour, min, sec;
    s32 date, month, year;
    s32 dow;

    u32 adc;
    u8 brightness;

    g_fault_code = FAULT_NONE;

    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&dow);

    /*
       RTC fault check.
    */
    if(hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59)
    {
        g_fault_code = FAULT_RTC_ERROR;
        return;
    }

    if(dow < 0 || dow > 6)
    {
        g_fault_code = FAULT_RTC_ERROR;
        return;
    }

    if(is_valid_date((u32)date, (u32)month, (u32)year) == 0)
    {
        g_fault_code = FAULT_RTC_ERROR;
        return;
    }

    /*
       LDR sensor fault check.
       ADC stuck at extreme value may indicate open/short condition.
    */
    adc = config_get_last_adc();

    if(adc <= 3 || adc >= 1020)
    {
        g_fault_code = FAULT_LDR_SENSOR_ERROR;
        return;
    }

    /*
       Low light warning:
       Brightness is already 100%, still system sees high darkness.
       This is only warning, not hardware fault.
    */
    brightness = config_get_brightness();

    if(brightness == 100)
    {
        g_fault_code = FAULT_LOW_LIGHT_WARNING;
        return;
    }
}

u8 fault_get_code(void)
{
    return g_fault_code;
}

char *fault_get_text(void)
{
    switch(g_fault_code)
    {
        case FAULT_NONE:
            return "NO FAULT";

        case FAULT_LDR_SENSOR_ERROR:
            return "LDR ERROR";

        case FAULT_RTC_ERROR:
            return "RTC ERROR";

        case FAULT_LOW_LIGHT_WARNING:
            return "LOW LIGHT";

        default:
            return "UNKNOWN";
    }
}
