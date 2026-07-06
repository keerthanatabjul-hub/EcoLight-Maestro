#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

extern volatile u8 g_task_100ms_flag;//keypad scan
extern volatile u8 g_task_500ms_flag;//lcd display
extern volatile u8 g_task_1sec_flag;//ldr read+brightness decision
extern volatile u8 g_task_5sec_flag;//fault check

void scheduler_timer1_init(void);
void timer1_isr(void) __irq;

#endif

