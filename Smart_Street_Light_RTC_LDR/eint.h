#ifndef EINT0_H
#define EINT0_H

extern volatile unsigned char g_eint0_flag;

void eint0_init(void);
void eint0_isr(void) __irq;

#endif

