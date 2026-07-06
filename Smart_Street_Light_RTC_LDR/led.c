#include <lpc21xx.h>
#include "led.h"
#include "defines.h"
#define STREET_LED_START       24
#define STREET_LED_MASK    (0xFFUL << STREET_LED_START)
void streetlight_led_init(void)
{
    IODIR1 |= STREET_LED_MASK;
	  //WRITEBYTE(IODIR1 ,STREET_LED_START,0xFF);
      IOSET1 = STREET_LED_MASK;
}

void streetlight_on(void)
{
	  IOCLR1 = STREET_LED_MASK;
    
}

void streetlight_off(void)
{
	  IOSET1 = STREET_LED_MASK;
    
}
