#ifndef TIMER0_H
#define TIMER0_H

void init_timer0(void);

void tdelay_us(unsigned int us);
void tdelay_ms(unsigned int ms);
void tdelay_s(unsigned int s);

#endif
