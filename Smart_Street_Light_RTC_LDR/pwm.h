#ifndef PWM_H
#define PWM_H

#include "types.h"

void pwm_init(void);
void pwm_set_brightness(u8 duty_percent);
u8 pwm_get_brightness(void);
void pwm_1ms_task(void);

#endif

