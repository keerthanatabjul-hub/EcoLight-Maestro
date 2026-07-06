#include "defines.h"
#include "delay.h"
#include "timer.h"
#include "lcd_defines.h"
#include<lpc21xx.h>
#include "types.h"
#include "lcd.h"
void Write_LCD(u8 data)
{
	SCLRBIT(IOCLR0,RW_PIN);
	//IOPIN0=(IOPIN0&~(255<<LCD_DATA))|(data<<LCD_DATA);
	WRITEBYTE(IOPIN0,LCD_DATA,data);
	//apply H to L pulse on EN_PIN
	SSETBIT(IOSET0,EN_PIN);
	tdelay_us(1);
	SCLRBIT(IOCLR0,EN_PIN);
  tdelay_ms(2);
}
void cmdlcd(u8 cmd)
{
	//select command reg RS=0
	SCLRBIT(IOCLR0,RS_PIN);
	//write command into lcd
	Write_LCD(cmd);
}
void charlcd(u8 ascii)
{
	//select data reg RS=1
	SSETBIT(IOSET0,RS_PIN);
	//write data into lcd
	Write_LCD(ascii);
}
void init_lcd(void)
{
	//cfg p0.0 to p0.7(lcd data_pins)
	WRITEBYTE(IODIR0,LCD_DATA,0xFF);
	//cfg p0.8,p0.9,p0.10(rs,rw,en pins)
	SETBIT(IODIR0,RS_PIN);
	SETBIT(IODIR0,RW_PIN);
	SETBIT(IODIR0,EN_PIN);
	//wait for 15 ms@ 5V
	tdelay_ms(15);
	cmdlcd(0x30);
	tdelay_ms(5);
	cmdlcd(0x30);
	tdelay_ms(100);
	cmdlcd(0x30);
	cmdlcd(0x38);//8bit mode 2 lines
	cmdlcd(0x0E);//display on cursor on
	cmdlcd(0x01);//clear lcd
	cmdlcd(0x06);//shift-cur-right
} 
void strlcd(char* str)
{
	while(*str)
	{
		charlcd(*str);
		str++;
	}
}
void u32lcd(u32 n)
{
	u8 a[10];
	s32 i=0;
	if(n==0)
	{
		charlcd('0');
	}
	else
	{
		while(n)
		{
			a[i++]=(n%10)+'0';
			n/=10;
		}
		for(--i;i>=0;i--)
		{
			charlcd(a[i]);
		}
	}
}
void s32lcd(s32 n)
{
	if(n<0)
	{
     charlcd('-');
		n=-n;
	}
	u32lcd(n);
}
void buildcgram(u8* p ,u32 nbytes)
{
	s32 i;
	cmdlcd(0x40);
	for(i=0;i<nbytes;i++)
	{
		charlcd(p[i]);
	}
	cmdlcd(0xC0);
}
void F32Lcd(f32 f,u8 ndp)
{
	u32 n,i;
	if(f<0.0)
	{
		charlcd('-');
		f=-f;
	}
	n=f;
	u32lcd(n);
	charlcd('.');
	for(i=0;i<ndp;i++)
	{
		f=(f-n)*10;
		n=f;
		charlcd(n+48);
	}

}
void BinLCD(int num)//binary value display
{
	int bitpos;
for(bitpos=7;bitpos>=0;bitpos--)
	{
		if(((num>>bitpos)&1))
		{
			charlcd('1');
		}
		else
		{
			charlcd('0');
		}
	}
}
void lcd_gotoxy(u8 row, u8 col)
{
    if(row == 0)
        cmdlcd(0x80 + col);
    else
        cmdlcd(0xC0 + col);
}

void lcd_clear(void)
{
    cmdlcd(0x01);
    delay_ms(2);
}

void lcd_print_2digit(u32 n)
{
    charlcd((n / 10) + '0');
    charlcd((n % 10) + '0');
}

void lcd_print_4digit(u32 n)
{
    charlcd(((n / 1000) % 10) + '0');
    charlcd(((n / 100)  % 10) + '0');
    charlcd(((n / 10)   % 10) + '0');
    charlcd((n % 10) + '0');
}
