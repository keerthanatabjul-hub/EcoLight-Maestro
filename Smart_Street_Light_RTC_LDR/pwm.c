#include "types.h"
#include "led.h"
#include "pwm.h"

/*
   Software PWM period = 10 ms.
   Timer1 ISR calls pwm_1ms_task() every 1 ms.

   0%   = OFF
   30%  = ON for 3 ms, OFF for 7 ms
   60%  = ON for 6 ms, OFF for 4 ms
   100% = Fully ON
*/

#define PWM_PERIOD_STEPS   10

static volatile u8 g_pwm_counter = 0;
static volatile u8 g_pwm_duty_steps = 0;
static volatile u8 g_pwm_percent = 0;

void pwm_init(void)
{
    g_pwm_counter = 0;
    g_pwm_duty_steps = 0;
    g_pwm_percent = 0;

    streetlight_off();
}

void pwm_set_brightness(u8 duty_percent)
{
    if(duty_percent > 100)
    {
        duty_percent = 100;
    }

    g_pwm_percent = duty_percent;

    if(duty_percent == 0)
    {
        g_pwm_duty_steps = 0;
    }
    else if(duty_percent >= 100)
    {
        g_pwm_duty_steps = PWM_PERIOD_STEPS;
    }
    else
    {
        g_pwm_duty_steps = duty_percent / 10;
    }
}

u8 pwm_get_brightness(void)
{
    return g_pwm_percent;
}

void pwm_1ms_task(void)
{
    g_pwm_counter++;

    if(g_pwm_counter >= PWM_PERIOD_STEPS)
    {
        g_pwm_counter = 0;
    }

    if(g_pwm_duty_steps == 0)
    {
        streetlight_off();
    }
    else if(g_pwm_duty_steps >= PWM_PERIOD_STEPS)
    {
        streetlight_on();
    }
    else
    {
        if(g_pwm_counter < g_pwm_duty_steps)
        {
            streetlight_on();
        }
        else
        {
            streetlight_off();
        }
    }
}

