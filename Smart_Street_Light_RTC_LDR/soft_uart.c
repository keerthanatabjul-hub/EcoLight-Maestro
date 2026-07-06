#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "soft_uart.h"
#include "pin_connect_block.h"
#include "pin_fun_defines.h"
/*
   Software UART pins

   For separate test code, you can use:
   #define TXD 0
   #define RXD 1

   For your Smart Street Light project, use free pins:
   TXD = P0.12
   RXD = P0.13
*/

#define TXD 12
#define RXD 13

#define BIT_TIME_US 104   /* 9600 baud */

void soft_uart_init(void)
{
    /*
       Configure P0.12 and P0.13 as GPIO.
       P0.12 uses PINSEL0 bits 25:24
       P0.13 uses PINSEL0 bits 27:26
    */
    //PINSEL0 &= ~(3 << 24);
    //PINSEL0 &= ~(3 << 26);
    cfgportpin(PORT0, PIN12, FUN1);
    cfgportpin(PORT0, PIN13, FUN1);
    /*
       TXD as output.
       RXD as input.
    */
    IODIR0 |=  (1 << TXD);
    IODIR0 &= ~(1 << RXD);

    /*
       UART idle state is HIGH.
    */
    IOSET0 = (1 << TXD);
}

void soft_uart_TX(u8 sdat)
{
    int i;
	u32 irq_backup;
	/* 
       Save currently enabled interrupts and disable them.
       Software UART needs exact 104us timing.
    */
    irq_backup = VICIntEnable;
    VICIntEnClr = irq_backup;
   
	 /*
       Start bit = LOW
    */
    IOCLR0 = (1UL << TXD);
    delay_us(BIT_TIME_US);

    /*
       Send 8 data bits, LSB first.
    */
    for(i = 0; i < 8; i++)
    {
        if(sdat & (1 << i))
        {
            IOSET0 = (1UL << TXD);
        }
        else
        {
            IOCLR0 = (1UL << TXD);
        }

        delay_us(BIT_TIME_US);
    }

    /*
       Stop bit = HIGH
    */
    IOSET0 = (1UL << TXD);
    delay_us(BIT_TIME_US);
		 /*
       Restore interrupts.
    */
    VICIntEnable = irq_backup;
}

u8 soft_uart_RX(void)
{
    u8 rdat = 0;
    int i;

    /*
       Wait for start bit.
       UART idle state is HIGH.
       Start bit is LOW.
    */
    while(((IOPIN0 >> RXD) & 1) == 1);

    /*
       Move to middle of first data bit.

       1 bit time  = 104 us
       Half bit    = 52 us

       Start edge to data bit0 center:
       104 + 52 = 156 us
    */
    delay_us(156);

    /*
       Receive 8 data bits, LSB first.
    */
    for(i = 0; i < 8; i++)
    {
        if((IOPIN0 >> RXD) & 1)
        {
            rdat |= (1 << i);
        }

        delay_us(BIT_TIME_US);
    }

    /*
       Wait for stop bit HIGH.
    */
    while(((IOPIN0 >> RXD) & 1) == 0);

    return rdat;
}

void soft_uart_str(char *str)
{
    while(*str)
    {
        soft_uart_TX((u8)*str);
        str++;
    }
}

void soft_uart_crlf(void)
{
    soft_uart_TX('\r');
    soft_uart_TX('\n');
}

void soft_uart_u32(u32 num)
{
    u8 arr[10];
    s32 i = 0;

    if(num == 0)
    {
        soft_uart_TX('0');
        return;
    }

    while(num)
    {
        arr[i++] = (num % 10) + '0';
        num = num / 10;
    }

    while(i > 0)
    {
        soft_uart_TX(arr[--i]);
    }
}



