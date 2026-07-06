#ifndef ALARM_H
#define ALARM_H

#include "types.h"

void alarm_init(void);
void alarm_check_task(void);
void alarm_buzzer_task_1ms(void);
u8 alarm_is_active(void);
#endif

