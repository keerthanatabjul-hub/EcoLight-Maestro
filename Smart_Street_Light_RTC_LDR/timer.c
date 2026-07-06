#include <lpc21xx.h>
#include "timer.h"

/*
   Assumption:
   FOSC = 12 MHz
   CCLK = 60 MHz
   PCLK = 15 MHz

   Timer clock = PCLK = 15 MHz

   For 1 us delay:
   15 counts = 1 us
   So PR = 14

   For 1 ms delay:
   15000 counts = 1 ms
   So PR = 14999

   For 1 sec delay:
   15000000 counts = 1 sec
   So PR = 14999999
*/

void init_timer0(void)
{
    /*
       Reset Timer0
    */
    T0TCR = (1 << 1);

    /*
       Stop timer when TC matches MR0.
       T0MCR bit 2 = Stop on MR0
    */
    T0MCR = (1 << 2);

    /*
       Keep timer disabled initially.
    */
    T0TCR = 0x00;
}

void tdelay_us(unsigned int us)
{
    /*
       Reset timer
    */
    T0TCR = (1 << 1);

    /*
       1 TC increment = 1 us
    */
    T0PR = 14;

    /*
       Required delay count
    */
    T0MR0 = us;

    /*
       Clear timer counter
    */
    T0TC = 0;

    /*
       Enable timer
    */
    T0TCR = (1 << 0);

    /*
       Wait until match value is reached
    */
    while(T0TC < T0MR0);

    /*
       Stop timer
    */
    T0TCR = 0x00;
}

void tdelay_ms(unsigned int ms)
{
    T0TCR = (1 << 1);

    /*
       1 TC increment = 1 ms
    */
    T0PR = 14999;

    T0MR0 = ms;
    T0TC = 0;

    T0TCR = (1 << 0);

    while(T0TC < T0MR0);

    T0TCR = 0x00;
}

void tdelay_s(unsigned int s)
{
    T0TCR = (1 << 1);

    /*
       1 TC increment = 1 second
    */
    T0PR = 14999999;

    T0MR0 = s;
    T0TC = 0;

    T0TCR = (1 << 0);

    while(T0TC < T0MR0);

    T0TCR = 0x00;
}
