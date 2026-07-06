#include "types.h"
#include "timer.h"
#include "defines.h"
#include "kpm_defines.h"
#include "kpm.h"
#include <lpc21xx.h>
#include "lcd.h"

/*u32 kpmlut[4][4]={{1,2,3,4},
                   {5,6,7,8},
									 {9,10,11,12},
									 {13,14,15,16}};*/
u8 kpmlut[4][4]={{'7','8','9','/'},
                   {'4','5','6','*'},
									 {'1','2','3','-'},
									 {'c','0','=','+'}};
void init_kpm(void)
{
	//cfg rows(p0.16 to p0.19) as o/p pins
	WRITENIBBLE(IODIR1,ROW0,15);
}
u32 colscan(void)
{
	if(READNIBBLE(IOPIN1,COL0)<15)//when switch is pressed
	{
		return 0;
	}
	else//when no switch is pressed
		return 1;
}
u32 rowcheck(void)
{
	u32 rno;     
	for(rno=0;rno<=3;rno++)
	{
		  WRITENIBBLE(IOPIN1,ROW0,~(1<<rno));
		  tdelay_ms(2);
	    if(colscan()==0)
			{
				break;
			}
	}
	//make rows as defaults
	WRITENIBBLE(IOPIN1,ROW0,0x0);
	return rno;
}
u32 colcheck(void)
{
	u32 cno;
	for(cno=0;cno<=3;cno++)
	{
		  if(READBIT(IOPIN1,(COL0+cno))==0)
				break;
	}
  
       
		
	return cno;
}
u32 keyscan(void)
{
	u32 rno,cno,keyv;
	//wait for switch press
	while(colscan());//while(0);
	tdelay_ms(20);
	//find rno								 
  rno=rowcheck();
	//find cno
	cno=colcheck();
	//take key from kpmlut
	keyv=kpmlut[rno][cno];
	//wait for switch release 
	while(!colscan());
	tdelay_ms(20);
	//return keyv
	return keyv;
}
s32 readnum(u8 *end_key)
{
	u8 key;
	s32 sum=0;
	u8 digit_entered=0;
	u8 sign_entered=0;
	 u8 digit_count = 0;
	s32 sign =1;
	while(1)
	{
		key=keyscan();
		/*if(key==99)
		{
			*end_key=key;
			return 0;
		}*/
		if((key=='-'||key=='+')&&(digit_entered==0)&&(sign_entered==0))
		{
			if(key=='-')
				sign=-1;
			else
			sign=1;
			sign_entered=1;
			charlcd(key);
		}
		else if(key>='0'&&key<='9')
		{
			sum=sum*10+(key-48);
			digit_entered=1;
			digit_count++;
			charlcd(key);
		}
		else if(key=='c')
		{
			if(digit_count>0)
			{
				sum=sum/10;
				digit_count--;
				if(digit_count==0)
				{
					digit_entered=0;
				}
				cmdlcd(0x10);
				charlcd(' ');
				cmdlcd(0x10);
			}
		else if(sign_entered==1)
		{
			sign_entered=0;
			sign=1;
			cmdlcd(0x10);
			charlcd(' ');
			cmdlcd(0x10);
		}
	}
		else if(key == '*')
        {
            sum = 0;
            digit_entered = 0;
            sign_entered = 0;
            digit_count = 0;
            sign = 1;

            /*
               Clear second line after VAL:
               Your app displays "VAL:" at column 0 to 3.
               So clear from column 4 onward.
            */
            cmdlcd(0xC0 + 4);
            strlcd("            ");
            cmdlcd(0xC0 + 4);
        }

		else
		{
			
			*end_key=key;
			if(digit_entered==0)
			{
				return READNUM_NO_INPUT;
			}
			break;
	  }
  }
	return sign*sum;			
}
u8 keyscan_nonblocking(void)
{
    u32 rno, cno;
    u8 keyv;

    /*
       If no key is pressed, return immediately.
    */
    if(colscan())
    {
        return 0;
    }

    /*
       Debounce delay.
    */
    tdelay_ms(20);

    /*
       Confirm key is still pressed.
    */
    if(colscan())
    {
        return 0;
    }

    rno = rowcheck();
    cno = colcheck();

    keyv = kpmlut[rno][cno];

    /*
       Wait for key release.
    */
    while(!colscan());

    tdelay_ms(20);

    return keyv;
}
/////////////////////////////////////////
/*key_scan timeout------wait a limited time in edit mode------
if not press anything for a 10s it will automatically go to the rtc mode
*/
////////////////////////////////////////
u8 keyscan_timeout(u32 timeout_ms)
{
    u32 elapsed_ms = 0;
    u8 key;

    while(elapsed_ms < timeout_ms)
    {
        key = keyscan_nonblocking();

        if(key != 0)
        {
            return key;
        }

        tdelay_ms(1);
        elapsed_ms++;
    }

    return 0;   /* timeout */
}
s32 readnum_timeout(u8 *end_key, u32 timeout_ms)
{
    u8 key;
    s32 sum = 0;
    u8 digit_entered = 0;
    u8 sign_entered = 0;
    u8 digit_count = 0;
    s32 sign = 1;

    while(1)
    {
        key = keyscan_timeout(timeout_ms);

        /*
           No key pressed within timeout.
        */
        if(key == 0)
        {
            *end_key = 0;
            return READNUM_TIMEOUT;
        }

        if((key == '-' || key == '+') && (digit_entered == 0) && (sign_entered == 0))
        {
            if(key == '-')
                sign = -1;
            else
                sign = 1;

            sign_entered = 1;
            charlcd(key);
        }
        else if(key >= '0' && key <= '9')
        {
            sum = (sum * 10) + (key - '0');

            digit_entered = 1;
            digit_count++;

            charlcd(key);
        }
        else if(key == 'c')
        {
            if(digit_count > 0)
            {
                sum = sum / 10;
                digit_count--;

                if(digit_count == 0)
                {
                    digit_entered = 0;
                }

                cmdlcd(0x10);
                charlcd(' ');
                cmdlcd(0x10);
            }
            else if(sign_entered == 1)
            {
                sign_entered = 0;
                sign = 1;

                cmdlcd(0x10);
                charlcd(' ');
                cmdlcd(0x10);
            }
        }
        else if(key == '*')
        {
            sum = 0;
            digit_entered = 0;
            sign_entered = 0;
            digit_count = 0;
            sign = 1;

            lcd_gotoxy(1, 4);
            strlcd("            ");
            lcd_gotoxy(1, 4);
        }
        else
        {
            *end_key = key;

            /*
               User pressed = without entering value.
            */
            if(digit_entered == 0)
            {
                return READNUM_NO_INPUT;
            }

            break;
        }
    }

    return sign * sum;
}

s32 readnum_timeout_digits(u8 *end_key, u32 timeout_ms, u8 max_digits)
{
    u8 key;
    s32 sum = 0;
    u8 digit_entered = 0;
    u8 sign_entered = 0;
    u8 digit_count = 0;
    s32 sign = 1;

    while(1)
    {
        key = keyscan_timeout(timeout_ms);

        if(key == 0)
        {
            *end_key = 0;
            return READNUM_TIMEOUT;
        }

        if((key == '-' || key == '+') && (digit_entered == 0) && (sign_entered == 0))
        {
            if(key == '-')
                sign = -1;
            else
                sign = 1;

            sign_entered = 1;
            charlcd(key);
        }
        else if(key >= '0' && key <= '9')
        {
            /*
               Digit limit check.
               Example:
               Month max digits = 2.
               If user enters third digit, return error.
            */
            if(digit_count >= max_digits)
            {
                *end_key = 0;
                return READNUM_DIGIT_LIMIT;
            }

            sum = (sum * 10) + (key - '0');

            digit_entered = 1;
            digit_count++;

            charlcd(key);
        }
        else if(key == 'c')
        {
            if(digit_count > 0)
            {
                sum = sum / 10;
                digit_count--;

                if(digit_count == 0)
                {
                    digit_entered = 0;
                }

                cmdlcd(0x10);
                charlcd(' ');
                cmdlcd(0x10);
            }
            else if(sign_entered == 1)
            {
                sign_entered = 0;
                sign = 1;

                cmdlcd(0x10);
                charlcd(' ');
                cmdlcd(0x10);
            }
        }
        else if(key == '*')
        {
            sum = 0;
            digit_entered = 0;
            sign_entered = 0;
            digit_count = 0;
            sign = 1;

            lcd_gotoxy(1, 4);
            strlcd("            ");
            lcd_gotoxy(1, 4);
        }
        else
        {
            *end_key = key;

            if(digit_entered == 0)
            {
                return READNUM_NO_INPUT;
            }

            break;
        }
    }

    return sign * sum;
}


