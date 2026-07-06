#include <lpc21xx.h>
#include "eint.h"
#include "pin_fun_defines.h"
#define EINT0_CHN0   14
#define EINT0_PIN    16

volatile unsigned char g_eint0_flag = 0;

void eint0_init(void)
{
    /*
       Configure P0.16 as EINT0 pin function.

       P0.16 is controlled by PINSEL1 bits 1:0.
       01 = EINT0 function.
    */
	//cfgportpin(PORT0,PIN16,FUN2);
    PINSEL1 = (PINSEL1 & ~(3 << ((PIN16 - 16) * 2))) |
              (1 << ((PIN16 - 16) * 2));

    /*
       Select EINT0 as IRQ
       Important:
       VICIntSelect = 0 << EINT0_CHN0 is wrong because it does nothing.entire thing will be cleared 
       Use &=~ to clear that bit.
    */
    VICIntSelect &= ~(1 << EINT0_CHN0);

    /*
       Enable EINT0 interrupt source.
    */
    VICIntEnable = (1 << EINT0_CHN0);

    /*
       Load ISR address into vector address register.
    */
    VICVectAddr0 = (unsigned int)eint0_isr;

    /*
       Enable slot 0 and assign EINT0 channel.
    */
    VICVectCntl0 = (1 << 5) | EINT0_CHN0;

    /*
       Select EINT0 as edge sensitive.
    */
    EXTMODE |= (1 << 0);

    /*
       Falling edge trigger.
       If you want rising edge, use:
       EXTPOLAR |= (1 << 0);
    */
    EXTPOLAR &= ~(1 << 0);

    /*
       Clear any pending EINT0 interrupt flag.
    */
    EXTINT = (1 << 0);
}

void eint0_isr(void) __irq
{
    /*
       Do not write LCD menu or delay inside ISR.
       Just set a flag.
    */
    g_eint0_flag = 1;

    /*
       Clear EINT0 interrupt flag.
    */
    EXTINT = (1 << 0);

    /*
       End of interrupt.
    */
    VICVectAddr = 0;
}

