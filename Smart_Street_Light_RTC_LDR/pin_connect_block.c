#include<lpc21xx.h>
#include "types.h"
void cfgportpin(u32 portno,u32 pinno,u32 pinfun)
{
	if(portno==0)
	{
		if(pinno<=15)
		{
			PINSEL0=(PINSEL0&~(3<<(pinno*2)))|(pinfun<<(pinno*2));
		}
		else if(pinno>=16&&pinno<=31)
		{
			PINSEL1=(PINSEL1&~(3<<((pinno-16)*2)))|(pinfun<<((pinno-16)*2));
		}
	}
}
