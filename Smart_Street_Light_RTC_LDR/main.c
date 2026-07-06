#include <lpc21xx.h>
#include "types.h"
#include "timer.h"
#include "delay.h"
#include "lcd.h"
#include "kpm.h"
#include "adc.h"
#include "rtc.h"
#include "led.h"
#include "eint.h"
#include "app.h"
#include "pwm.h"
#include "scheduler.h"
#include "config.h"
#include "motion.h"
#include "alarm.h"
#include "date_utils.h"
#include "ui_mannager.h"
#include "event_logger.h"
#include "soft_uart.h"


#include "i2c.h"
#include "soft_uart.h"
#include "config_storage.h"
#include "uart_monitor.h"
#include "zone_light_spi.h"

#define LOAD_DEMO_RTC_ON_START   1

int main(void)
{
	   alarm_init();
	  init_timer0();
    init_lcd();
	  app_load_custom_symbols();
	ui_manager_init();
	  config_init();
	
   event_logger_init();
	zone_spi_light_init();
    streetlight_led_init();
	  pwm_init(); 
	  init_kpm();
    init_adc();
    rtc_init();
    eint0_init();
	motion_init();
	 soft_uart_init();
	soft_uart_str("UART TEST ok");
soft_uart_crlf();
	soft_uart_str("ECOLIGHT SYSTEM STARTED");
soft_uart_crlf();
    Init_I2C();
    config_storage_load();
	  soft_uart_init();
		#if LOAD_DEMO_RTC_ON_START

{
    u32 demo_date  = 17;
    u32 demo_month = 6;
    u32 demo_year  = 2026;
    u8 demo_day;

    SetRTCTimeInfo(22, 23, 0);

    SetRTCDateInfo(
        demo_date,
        demo_month,
        demo_year
    );

    demo_day = calculate_day_of_week(
        demo_date,
        demo_month,
        demo_year
    );

    SetRTCDay(demo_day);
}

#endif
		app_show_startup_message();

    scheduler_timer1_init();
		
#if LOAD_DEMO_RTC_ON_START

    SetRTCTimeInfo(17, 59, 50);
    SetRTCDateInfo(16, 5, 2026);
    SetRTCDay(SAT);

#endif

   // app_show_startup_message();
      /*
       Timer1 scheduler starts after startup message.
    */
    //scheduler_timer1_init();
    while(1)
    {
        if(g_eint0_flag == 1)
        {
            g_eint0_flag = 0;
            
					if(app_check_admin_password() == 1)
           {
             app_rtc_menu_process();
             config_storage_save();
           }

            lcd_clear();
				}
            

        if(g_task_100ms_flag == 1)
        {
            g_task_100ms_flag = 0;
					 ui_task_100ms();
            app_keypad_task();
					  zone_spi_motion_task_100ms();
					
        }

        if(g_task_500ms_flag == 1)
        {
            g_task_500ms_flag = 0;
					 if((ui_is_message_active() == 0) && (alarm_is_active() == 0))
					 {
            app_display_task();
					 }
        }

        if(g_task_1sec_flag == 1)
        {
            g_task_1sec_flag = 0;
            app_zone_spi_light_task();
					   alarm_check_task();
        }

        if(g_task_5sec_flag == 1)
        {
            g_task_5sec_flag = 0;
            app_fault_task();
					
            /*
               UART debug/status to ESP32 or Virtual Terminal.
            */
            uart_monitor_send_status();
        }
    }
}

