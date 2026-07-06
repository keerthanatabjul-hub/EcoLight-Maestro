#ifndef APP_H
#define APP_H
#include "types.h"
void app_status_screen(void);


void app_show_startup_message(void);
void app_normal_task(void);
void app_rtc_menu_process(void);
void app_load_custom_symbols(void);
void app_display_task(void);
void app_streetlight_task(void);
void app_keypad_task(void);
void app_fault_task(void);
void app_status_screen(void);
void app_zone_spi_light_task(void);
u8 app_check_admin_password(void);
#endif
