#include "types.h"
void cmdlcd(u8 cmd);
void Write_LCD(u8 data);
void cmdlcd(u8 cmd);
void charlcd(u8 ascii);
void init_lcd(void);
void strlcd(char* str);
void s32lcd(s32 n);
void u32lcd(u32 n);
void buildcgram(u8 *p ,u32 nbytes);
void F32Lcd(f32 f,u8 ndp);
void BinLCD(int num);

//new modification
void lcd_gotoxy(u8 row, u8 col);
void lcd_clear(void);
void lcd_print_2digit(u32 n);
void lcd_print_4digit(u32 n);

