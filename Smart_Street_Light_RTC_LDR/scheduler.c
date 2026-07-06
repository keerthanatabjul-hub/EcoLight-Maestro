#include <lpc21xx.h>
#include "types.h"
#include "scheduler.h"
#include "alarm.h"
#include "zone_light_spi.h"
#define TIMER1_VIC_CHANNEL   5

/* Timer clock assumption:
CCLK = 60 MHz 
PCLK = 15 MHz 
T1PR = 14: 
Timer counter increments once every 1 microsecond. 
T1MR0 = 100: 
Timer interrupt occurs every 100 microseconds. */


volatile u8 g_task_100ms_flag = 0;
volatile u8 g_task_500ms_flag = 0;
volatile u8 g_task_1sec_flag  = 0;
volatile u8 g_task_5sec_flag  = 0;

static volatile u32 g_ms_count = 0;
static volatile u8 g_100us_count = 0;
/* Function prototype is required because the ISR address 
is used before the function definition. */ 
void timer1_isr(void) __irq;
void scheduler_timer1_init(void)
{
    /*
      Stop and Reset Timer1
    */
    T1TCR = (1UL << 1);
	/* Explicitly clear timer and prescale counters. */ 
	T1TC = 0; 
	T1PC = 0;

    /* PCLK = 15 MHz. 
	Prescaler division = T1PR + 1 
	= 14 + 1 
	= 15 
	15 MHz / 15 = 1 MHz 
	Therefore Timer1 TC increments every 1 us. */
    T1PR = 14;

    /* Match after 100 timer counts.
	100 × 1 us = 100 us. */
    T1MR0 = 100;

    /*
       Interrupt and reset on MR0
    */
    T1MCR = (1UL << 0) | (1UL << 1);

    /*
       Timer1 as IRQ
    */
    VICIntSelect &= ~(1UL << TIMER1_VIC_CHANNEL);

    /*
       Use VIC slot 1.
       EINT0 can use slot 0.
    */
    VICVectAddr1 = (unsigned int)timer1_isr;
    VICVectCntl1 = (1UL << 5) | TIMER1_VIC_CHANNEL;

    VICIntEnable = (1UL << TIMER1_VIC_CHANNEL);

    /*
       Clear Timer1 interrupt flag
    */
    T1IR = 0x01;

    /*
       Start Timer1
    */
    T1TCR = (1UL << 0);
}

void timer1_isr(void) __irq
{
    /*
       Clear Timer1 MR0 interrupt flag
    */
    T1IR = 0x01;
   /*
       Every 100 us.
    */
    zone_spi_pwm_task_100us();

    g_100us_count++;

    if(g_100us_count >= 10)
    {
        g_100us_count = 0;

	
    /*
       This ISR executes every 1 ms.
    */
    alarm_buzzer_task_1ms();
     g_ms_count++;

    if((g_ms_count % 100) == 0)
    {
        g_task_100ms_flag = 1;
    }

    if((g_ms_count % 500) == 0)
    {
        g_task_500ms_flag = 1;
    }

    if((g_ms_count % 1000) == 0)
    {
        g_task_1sec_flag = 1;
    }

    if((g_ms_count % 5000) == 0)
    {
        g_task_5sec_flag = 1;
    }
/* Prevent the counter from continuously increasing. */
    if(g_ms_count >= 60000)
    {
        g_ms_count = 0;
    }
	}
    /*
       Signal the end of the interrupt.
    */
    VICVectAddr = 0;
}

