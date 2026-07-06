#include <lpc21xx.h>
#include "types.h"
#include "zone_light_spi.h"
#include "spi_shift.h"
#include "adc.h"
#include "config.h"
#include "event_logger.h"
#include "brightness.h"
/*
   74HC595 output mapping:

   Zone 0 -> Q0, Q1
   Zone 1 -> Q2, Q3
   Zone 2 -> Q4, Q5
   Zone 3 -> Q6, Q7
*/

#define ZONE0_MASK      0x03      /* Q0 Q1 */
#define ZONE1_MASK      0x0C      /* Q2 Q3 */
#define ZONE2_MASK      0x30      /* Q4 Q5 */
#define ZONE3_MASK      0xC0      /* Q6 Q7 */



#define MOTION_HOLD_COUNT    100  /* 100 x 100ms = 10 sec */

#define MOTION_ACTIVE_LEVEL  1

#define ZONE0_MOTION_PIN     23
#define ZONE1_MOTION_PIN     24
#define ZONE2_MOTION_PIN     25
#define ZONE3_MOTION_PIN     28

#define LDR_ADC_CHANNEL_SELECT   (1 << 0)

#define BRIGHTNESS_OFF       0
#define BRIGHTNESS_LOW       30
#define BRIGHTNESS_MEDIUM    60
#define BRIGHTNESS_HIGH      100

/*
   If your LEDs are connected:
   Qx -> resistor -> LED -> GND
   keep this as 1.
*/
#define ZONE_LED_ACTIVE_HIGH  0

static const u8 zone_mask[ZONE_COUNT] =
{
    ZONE0_MASK,
    ZONE1_MASK,
    ZONE2_MASK,
    ZONE3_MASK
};
/*
   Add the new arrays here.
*/


static const u8 zone_motion_pin[ZONE_COUNT] =
{
    ZONE0_MOTION_PIN,
    ZONE1_MOTION_PIN,
    ZONE2_MOTION_PIN,
    ZONE3_MOTION_PIN
};

static volatile u8 zone_brightness[ZONE_COUNT] = {0, 0, 0, 0};
#define PWM_PERIOD_STEPS 10

static volatile u8 zone_pwm_steps[ZONE_COUNT] =
{
    0, 0, 0, 0
};

static volatile u8 pwm_counter = 0;

/*
   Avoid repeatedly writing the same byte to 74HC595.
   This reduces CPU load.
*/
static u8 last_shift_data = 0xFF;

static u8 zone_motion_active[ZONE_COUNT] = {0, 0, 0, 0};
static u16 zone_motion_counter[ZONE_COUNT] = {0, 0, 0, 0};

static u32 read_ldr_average(void)
{
    u32 i;
    u32 adc_value;
    u32 sum = 0;
    f32 analog_voltage;

    for(i = 0; i < 10; i++)
    {
        read_adc(LDR_ADC_CHANNEL_SELECT, &adc_value, &analog_voltage);
        sum = sum + adc_value;
    }

    return sum / 10;
}

void zone_spi_light_init(void)
{
    u8 zone;

    spi_shift_init();

    /*
       Configure P0.23 to P0.26 as GPIO input for zone motion sensors.
       These pins are in PINSEL1.
    */
    PINSEL1 &= ~(3UL << ((ZONE0_MOTION_PIN - 16) * 2));
    PINSEL1 &= ~(3UL << ((ZONE1_MOTION_PIN - 16) * 2));
    PINSEL1 &= ~(3UL << ((ZONE2_MOTION_PIN - 16) * 2));
    PINSEL1 &= ~(3UL << ((ZONE3_MOTION_PIN - 16) * 2));

    for(zone = 0; zone < ZONE_COUNT; zone++)
    {
        IODIR0 &= ~(1UL << zone_motion_pin[zone]);

        zone_brightness[zone] = BRIGHTNESS_OFF;
        
        zone_pwm_steps[zone] = 0;
        zone_motion_active[zone] = 0;
        zone_motion_counter[zone] = 0;
    }

   pwm_counter = 0;
    last_shift_data = 0xFF;

    /*
       Active-low LEDs:
       0xFF means all outputs HIGH and all LEDs OFF.
    */
    spi_shift_write_byte(0xFF);
}

void zone_spi_pwm_task_100us(void)
{
    u8 zone;
    u8 led_on_mask = 0;
    u8 shift_data;

    /*
       Check every zone.

       zone_mask contains both LEDs:

       Zone 0 -> Q0 and Q1
       Zone 1 -> Q2 and Q3
       Zone 2 -> Q4 and Q5
       Zone 3 -> Q6 and Q7
    */
    for(zone = 0; zone < ZONE_COUNT; zone++)
    {
        if(pwm_counter < zone_pwm_steps[zone])
        {
            /*
               Both LEDs of this zone become ON
               during the PWM ON portion.
            */
            led_on_mask |= zone_mask[zone];
        }
    }

#if ZONE_LED_ACTIVE_HIGH

    shift_data = led_on_mask;

#else

    /*
       Your LEDs are active-low.

       Logical ON bit 1 becomes physical output 0.
    */
    shift_data = (u8)(~led_on_mask);

#endif

    /*
       Write only when output changes.
       This reduces unnecessary SPI shifting.
    */
    if(shift_data != last_shift_data)
    {
        spi_shift_write_byte(shift_data);
        last_shift_data = shift_data;
    }

    pwm_counter++;

    if(pwm_counter >= PWM_PERIOD_STEPS)
    {
        pwm_counter = 0;
    }
}

void zone_spi_set_brightness(u8 zone, u8 brightness)
{
    if(zone >= ZONE_COUNT)
    {
        return;
    }

    /*
       Accept only the supported brightness levels.
    */
    if((brightness != BRIGHTNESS_OFF) &&
       (brightness != BRIGHTNESS_LOW) &&
       (brightness != BRIGHTNESS_MEDIUM) &&
       (brightness != BRIGHTNESS_HIGH))
    {
        return;
    }
 /*
       Avoid writing repeatedly when brightness did not change.
    */
		if(zone_brightness[zone] == brightness)
		{
			return;
		}
    zone_brightness[zone] = brightness;
    /*
       Convert percentage into 10-step PWM duty cycle.

       0   -> 0 steps
       30  -> 3 steps
       60  -> 6 steps
       100 -> 10 steps
    */
   zone_pwm_steps[zone] =
        (u8)((brightness * PWM_PERIOD_STEPS) / 100);
}

u8 zone_spi_get_brightness(u8 zone)
{
    if(zone >= ZONE_COUNT)
    {
        return 0;
    }

    return zone_brightness[zone];
}

u8 zone_spi_get_display_brightness(void)
{
    u8 zone;
    u8 max_brightness = 0;

    for(zone = 0; zone < ZONE_COUNT; zone++)
    {
        if(zone_brightness[zone] > max_brightness)
        {
            max_brightness = zone_brightness[zone];
        }
    }

    return max_brightness;
}



void zone_spi_motion_task_100ms(void)
{
    u8 zone;
    u8 pin_state;

    for(zone = 0; zone < ZONE_COUNT; zone++)
    {
        pin_state = (IOPIN0 >> zone_motion_pin[zone]) & 1;

        if(pin_state == MOTION_ACTIVE_LEVEL)
        {
            if(zone_motion_active[zone] == 0)
            {
                event_log(EVENT_MOTION_DETECTED);
            }

            zone_motion_active[zone] = 1;
            zone_motion_counter[zone] = MOTION_HOLD_COUNT;
        }

        if(zone_motion_counter[zone] > 0)
        {
            zone_motion_counter[zone]--;

            if(zone_motion_counter[zone] == 0)
            {
                zone_motion_active[zone] = 0;
            }
        }
    }
}

u8 zone_spi_motion_is_active(u8 zone)
{
    if(zone >= ZONE_COUNT)
    {
        return 0;
    }

    return zone_motion_active[zone];
}

void zone_spi_control_task(u32 hour, u32 min)
{
    u8 zone;
    u8 mode;
    u8 base_brightness;
    u8 display_brightness;
    u32 adc_value;

    mode = config_get_mode();

    /*
       MANUAL ON:
       All zones full brightness.
    */
    if(mode == MODE_MANUAL_ON)
    {
        for(zone = 0; zone < ZONE_COUNT; zone++)
        {
					 
					
            zone_spi_set_brightness(zone, BRIGHTNESS_HIGH);
						
        }
        config_set_brightness(BRIGHTNESS_HIGH);
        return;
    }
					

    /*
       MANUAL OFF:
       All zones OFF.
    */
    if(mode == MODE_MANUAL_OFF)
    {
        for(zone = 0; zone < ZONE_COUNT; zone++)
        {
            zone_spi_set_brightness(zone, BRIGHTNESS_OFF);
        }

        config_set_brightness(BRIGHTNESS_OFF);    /* LCD B:0 */
       
				return;
    }

    /*
       AUTO mode:
       If current time is outside ON/OFF schedule,
       all lights should remain OFF.
    */
    if(config_is_light_schedule_active((u8)hour, (u8)min) == 0)
    {
        for(zone = 0; zone < ZONE_COUNT; zone++)
        {
            zone_spi_set_brightness(zone, BRIGHTNESS_OFF);
        }

        config_set_brightness(BRIGHTNESS_OFF);    /* LCD B:0 */
       
				return;
    }

    /*
       Read LDR ADC value.
    */
    adc_value = read_ldr_average();

    /*
       Store latest ADC value for status screen.
    */
    config_set_last_adc(adc_value);

    /*
       Use already existing brightness decision function.

       This function should return only:
       0, 30, 60, 100
    */
    base_brightness = brightness_get_from_adc(adc_value);

    /*
       Default LCD brightness is LDR-based brightness.
    */
    display_brightness = base_brightness;

    /*
       Zone-wise motion control:
       If motion is detected in one zone,
       only that zone becomes 100%.
    */
    for(zone = 0; zone < ZONE_COUNT; zone++)
    {
        if(zone_motion_active[zone]!=0)
        {
            zone_spi_set_brightness(zone, BRIGHTNESS_HIGH);

            /*
               LCD should show 100 if any one zone is boosted.
            */
            display_brightness = BRIGHTNESS_HIGH;
        }
        else
        {
            zone_spi_set_brightness(zone, base_brightness);
        }
    }

    /*
       LCD brightness value:
       No motion  -> 0 / 30 / 60 / 100 based on LDR
       Motion     -> 100
    */
    config_set_brightness(display_brightness);
   
}

