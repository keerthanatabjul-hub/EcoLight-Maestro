#include<lpc21xx.h>
#include "adc_defines.h"
#include "pin_fun_defines.h"
#include "pin_connect_block.h"
#include "timer.h"
#include "delay.h"
#include "types.h"
void init_adc(void)
 {
	//make p0.27 as gpio
	PINSEL1&=~(3<<((27-16)*2));
	//cfg p0.27 as AIN0 pin
	//PINSEL1&=~(3<<((27-16)*2))|(1<<((27-16)*2));
	cfgportpin(PORT0,PIN27,FUN2);
	//activate the ADC peripheral,set adc clk freq
 	ADCR=PDN_BIT|CLKDIV_VALUE;
}
void read_adc(u32 chno,u32* adcdval,f32* ear)
  {
	//   clear the channel bits in ADCR
	ADCR&=~(255<<0);
	//select channel no and start conv
	ADCR|=chno|START_CONV;  
	//wait for conv upto 3usec
	tdelay_us(3);
	//wait for the done bit status 
	while(( (ADDR>>DONE_BIT)&1)==0);
	//stop the convertion
  ADCR&=~	(START_CONV);
	//extract the result from ADDR
	*adcdval=((ADDR>>RESULT)&0x3FF);
	//get the equivalent analog reading
	*ear=(3.3/1023)*(*adcdval);
}
