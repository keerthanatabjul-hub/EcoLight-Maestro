#include "types.h"
#include "adc.h"
#include "led.h"
#include "streetlight.h"
#include "pwm.h"
#include "config.h"
#include "motion.h"
#include "event_logger.h"
/*
   Your LDR is connected to AIN0 / P0.27.
   Your read_adc() function expects channel select bit.

   AIN0 -> (1 << 0)
   AIN1 -> (1 << 1)
   AIN2 -> (1 << 2)
*/
#define LDR_ADC_CHANNEL_SELECT   (1 << 0)

/*
   Adjust this threshold after checking actual LDR ADC value.
*/
//Hysteresis thresholds
/*

If threshold is 350, LED may rapidly ON/OFF around 348, 352, 349.

Use two thresholds:

LED ON  when ADC < 300
LED OFF when ADC > 400
Why this is good

This prevents unwanted blinking.*/
#define HIGH_DARK_THRESHOLD       300
#define MEDIUM_DARK_THRESHOLD       600
#define LOW_DARK_THRESHOLD       850
/*
   Night time range:
   18 means 6 PM.
   6 means 6 AM.
*/
#define NIGHT_START_HOUR         18
#define NIGHT_END_HOUR           6

/*
   Set this based on your LDR circuit.

   If ADC value is HIGH in darkness, make this 1.
   If ADC value is LOW in darkness, make this 0.
*/
#define LDR_DARK_IS_HIGH_ADC     0
#define BRIGHTNESS_OFF           0
#define BRIGHTNESS_LOW           30
#define BRIGHTNESS_MEDIUM        60
#define BRIGHTNESS_HIGH          100


u8 is_night_time(u32 hour)
{
    if(hour >= NIGHT_START_HOUR || hour < NIGHT_END_HOUR)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
//read average ldr 
/*
Currently you read ADC once:

read_adc(LDR_ADC_CHANNEL_SELECT, &adc_value, &analog_voltage);

But real sensors fluctuate. So read 5 or 10 samples and take average.

Why this is good

It avoids LED flickering due to small LDR noise.

Example:

adc_value = read_ldr_average();*/
static u32 read_ldr_avg(void)
{
	u32 i;
	u32 adc_value;
	u32 sum=0;
	f32 analog_voltage;
	for(i=0;i<5;i++)
	{
		read_adc(LDR_ADC_CHANNEL_SELECT, &adc_value, &analog_voltage);
		sum=sum+adc_value;
	}
	return sum/5;
}
void streetlight_control(u32 hour,u32 min)
{
	u32 threshold_100;
u32 threshold_60;
u32 threshold_30;
static u8 last_brightness = 255;
	
	
	
    u32 adc_value;
	  u8 mode;
    u8 brightness;
	  mode = config_get_mode();
	 /*
       Manual ON has highest priority.
    */
    if(mode == MODE_MANUAL_ON)
    {
        brightness = BRIGHTNESS_HIGH;

        pwm_set_brightness(brightness);
        config_set_brightness(brightness);

        return;
    }

    /*
       Manual OFF has highest priority.
    */
    if(mode == MODE_MANUAL_OFF)
    {
        brightness = BRIGHTNESS_OFF;

        pwm_set_brightness(brightness);
        config_set_brightness(brightness);

        return;
    }

    /*
       AUTO mode:
       First check user-configured ON/OFF schedule.
    */
    if(config_is_light_schedule_active((u8)hour, (u8)min) == 0)
    {
        brightness = BRIGHTNESS_OFF;

        pwm_set_brightness(brightness);
        config_set_brightness(brightness);

        return;
    }

		/*
   If motion is detected at night, force 100% brightness.
*/
    if(motion_is_active())
    {
     brightness = BRIGHTNESS_HIGH;
     pwm_set_brightness(brightness);
    config_set_brightness(brightness);
    return;
    }
    //f32 analog_voltage;

    /*
       Streetlight control only between:
       18:00 to 23:59
       and
       00:00 to 05:59
    */
   /*  if(is_night_time(hour) == 0)
    {
        pwm_set_brightness(BRIGHTNESS_OFF);
        return;
    }*/
        /*
           Read LDR value from ADC channel AIN0.
           This calls your adc.c function:
           read_adc(u32 chno, u32 *adcdval, f32 *ear)
        */
			adc_value= read_ldr_avg();
       // read_adc(LDR_ADC_CHANNEL_SELECT, &adc_value, &analog_voltage);
       config_set_last_adc(adc_value);
			 
			 threshold_100 = config_get_threshold_100();
threshold_60  = config_get_threshold_60();
threshold_30  = config_get_threshold_30();
#if LDR_DARK_IS_HIGH_ADC

        /*
           Case 1:
           If your LDR circuit gives high ADC value in darkness.
        */
        if(adc_value > threshold_30 )
        {
            brightness = BRIGHTNESS_HIGH;
        }
        else if(adc_value > threshold_60)
        {
            brightness = BRIGHTNESS_MEDIUM;
        }
				else if(adc_value > threshold_100)
        {
        brightness = BRIGHTNESS_LOW;
        }
				else
        {
        brightness = BRIGHTNESS_OFF;
        }

#else

        /*
           Case 2:
           If your LDR circuit gives low ADC value in darkness.
           This matches your project statement:
           intensity below threshold -> LEDs ON.
        */
          if(adc_value < threshold_100)
    {
       brightness = BRIGHTNESS_HIGH;
		}
    else if(adc_value < threshold_60)
    {
        brightness = BRIGHTNESS_MEDIUM;
    }
    else if(adc_value < threshold_30)
    {
         brightness = BRIGHTNESS_LOW;
    }
    else
    {
       brightness = BRIGHTNESS_OFF;
    }


#endif
   pwm_set_brightness(brightness);
    config_set_brightness(brightness);
		
		
		if(brightness != last_brightness)
    {
        event_log(EVENT_BRIGHTNESS_CHANGED);
        last_brightness = brightness;
    }
}

