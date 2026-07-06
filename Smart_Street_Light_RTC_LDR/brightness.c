#include "types.h"
#include "config.h"
#include "brightness.h"

u8 brightness_get_from_adc(u32 adc_value)
{
    u32 threshold_100;
    u32 threshold_60;
    u32 threshold_30;

    threshold_100 = config_get_threshold_100();
    threshold_60  = config_get_threshold_60();
    threshold_30  = config_get_threshold_30();

    /*
       Your LDR connection:
       3.3V -> LDR -> ADC -> 10k -> GND

       More light  -> ADC high
       Darkness    -> ADC low
    */

    if(adc_value < threshold_100)
    {
        return BRIGHTNESS_HIGH;      /* 100 */
    }
    else if(adc_value < threshold_60)
    {
        return BRIGHTNESS_MEDIUM;    /* 60 */
    }
    else if(adc_value < threshold_30)
    {
        return BRIGHTNESS_LOW;       /* 30 */
    }
    else
    {
        return BRIGHTNESS_OFF;       /* 0 */
    }
}
