#include "types.h"
#include "lcd.h"
#include "ui_mannager.h"

static u8 g_ui_msg_active = 0;
static u32 g_ui_msg_time_ms = 0;

void ui_manager_init(void)
{
    g_ui_msg_active = 0;
    g_ui_msg_time_ms = 0;
}

void ui_show_message(char *line1, char *line2, u32 duration_ms)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd(line1);

    lcd_gotoxy(1, 0);
    strlcd(line2);

    g_ui_msg_active = 1;
    g_ui_msg_time_ms = duration_ms;
}

void ui_task_100ms(void)
{
    if(g_ui_msg_active == 0)
    {
        return;
    }

    if(g_ui_msg_time_ms > 100)
    {
        g_ui_msg_time_ms = g_ui_msg_time_ms - 100;
    }
    else
    {
        g_ui_msg_time_ms = 0;
        g_ui_msg_active = 0;
        lcd_clear();
    }
}

u8 ui_is_message_active(void)
{
    return g_ui_msg_active;
}

