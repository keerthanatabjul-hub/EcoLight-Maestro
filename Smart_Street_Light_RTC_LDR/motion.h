#ifndef MOTION_H
#define MOTION_H

#include "types.h"

void motion_init(void);
void motion_task_100ms(void);
u8 motion_is_active(void);
void motion_trigger(void);

#endif


