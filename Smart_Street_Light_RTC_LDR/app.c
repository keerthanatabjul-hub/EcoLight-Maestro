#include "types.h"
#include "timer.h"
#include "delay.h"
#include "lcd.h"
#include "kpm.h"
#include "pwm.h"
#include "rtc.h"
#include "date_utils.h"
#include "streetlight.h"
#include "led.h"
#include "app.h"
#include "config.h"
#include "fault.h"
#include "motion.h"
#include "zone_light_spi.h"
#include "ui_mannager.h"
#include "event_logger.h"
#include "zone_light_spi.h"
#include "config_storage.h"
#define ADMIN_PASSWORD       1234
#define MAX_PASSWORD_TRIES   3
#define MENU_TIMEOUT_MS        10000
#define VALUE_TIMEOUT_MS       10000
static void app_set_on_time_process(void);
static void app_set_off_time_process(void);
static void lcd_timeout_message(void);
static u8 last_modified_valid = 0;
//update calibration_process
static void app_calibration_process(void);

static s32 lm_hour = 0;
static s32 lm_min  = 0;
static s32 lm_sec  = 0;

static s32 lm_date  = 0;
static s32 lm_month = 0;
static s32 lm_year  = 0;
/*
   CGRAM character 0: Existing light/bulb symbol
   CGRAM character 1: New power symbol
*/
static u8 custom_symbols[16] =
{
    /* Character 0: existing light symbol */
    0x04,
    0x0E,
    0x0E,
    0x0E,
    0x1F,
    0x1F,
    0x0E,
    0x04,

    /* Character 1: power symbol */
    0x04,
    0x04,
    0x15,
    0x11,
    0x11,
    0x0E,
    0x00,
    0x00
};
static char *day_name[] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};
void app_load_custom_symbols(void)
{
    /*
       Load two symbols:
       character 0 = light symbol
       character 1 = power symbol
    */
    buildcgram(custom_symbols, 16);
}
/* =========================================================
   LCD HELPER FUNCTIONS
   ========================================================= */

static void lcd_print_2digit_local(u32 n)
{
    charlcd((n / 10) + '0');
    charlcd((n % 10) + '0');
}

static void lcd_print_4digit_local(u32 n)
{
    charlcd(((n / 1000) % 10) + '0');
    charlcd(((n / 100)  % 10) + '0');
    charlcd(((n / 10)   % 10) + '0');
    charlcd((n % 10) + '0');
}

static void lcd_message(char *line1, char *line2 ,u32 dly_ms)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd(line1);

    lcd_gotoxy(1, 0);
    strlcd(line2);

    tdelay_ms(dly_ms);
}

void app_show_startup_message(void)
{
	
    lcd_clear();
  	lcd_gotoxy(0, 0);
    strlcd("SMART STREET");

    lcd_gotoxy(1, 0);
    strlcd("LIGHT SYSTEM");
    lcd_gotoxy(1, 14);
    charlcd(0);
    tdelay_ms(1500);

	  
	
}

/* =========================================================
   DISPLAY RTC INFO
   Uses your modified RTC functions indirectly.
   ========================================================= */

static void display_rtc_info(
    u32 hour,
    u32 min,
    u32 sec,
    u32 date,
    u32 month,
    u32 year,
    u32 dow
)
{
    //lcd_clear();

    if(dow > 6)
        dow = 0;

    /*
       Line 1:
       HH:MM:SS DAY
    */
    lcd_gotoxy(0, 0);

    lcd_print_2digit_local(hour);
    charlcd(':');

    lcd_print_2digit_local(min);
    charlcd(':');

    lcd_print_2digit_local(sec);
    charlcd(' ');

    strlcd(day_name[dow]);
		 strlcd("    ");

    /*
       Line 2:
       DD/MM/YYYY
    */
    lcd_gotoxy(1, 0);

    lcd_print_2digit_local(date);
    charlcd('/');

    lcd_print_2digit_local(month);
    charlcd('/');

    lcd_print_4digit_local(year);
		  strlcd("      ");
}
/*======================================================================
get digit limit based on max limit in hour (or) min (or) year etc...
for example hour take 2 digits our max limit 23 so the digits is <100 
so returning 2
========================================================================*/
static u8 get_digit_limit_from_max(s32 max)
{
    if(max < 10)
    {
        return 1;
    }
    else if(max < 100)
    {
        return 2;
    }
    else if(max < 1000)
    {
        return 3;
    }
    else if(max < 10000)
    {
        return 4;
    }
    else
    {
        return 10;
    }
}
/* =========================================================
   KEYPAD VALIDATED NUMBER INPUT
   Your keypad confirms using '='.
   ========================================================= */

static s32 read_valid_number(char *title, s32 min, s32 max)
{
    s32 value;
    u8 end_key;
    u8 max_digits;

    max_digits = get_digit_limit_from_max(max);
    while(1)
    {
        lcd_clear();

        lcd_gotoxy(0, 0);
        strlcd(title);

        lcd_gotoxy(1, 0);
        strlcd("VAL:");

        value = readnum_timeout_digits(&end_key,VALUE_TIMEOUT_MS,max_digits);
          /*
           No key pressed for timeout period.
        */
        if(value == READNUM_TIMEOUT)
        {
            lcd_timeout_message();
            return READNUM_TIMEOUT;
        }
				if(value == READNUM_DIGIT_LIMIT)
        {
            lcd_message("TOO MANY DIGITS", "TRY AGAIN", 1000);
            continue;
        }
        if(end_key != '=')
        {
            lcd_message("PRESS = TO OK", "TRY AGAIN", 1200);
            continue;
        }
        if(value == READNUM_NO_INPUT)
				{
					lcd_message("INVALID INPUT","ENTER VALUE",1000);
					continue;
				}
        if(value < min || value > max)
        {
            lcd_message("INVALID INPUT", "TRY AGAIN", 1200);
            continue;
        }

        return value;
    }
}
static void save_last_modified_time(void)
{
    /*
       Store current RTC time/date as last modified time.
       This is called whenever user updates RTC field.
    */
    GetRTCTimeInfo(&lm_hour, &lm_min, &lm_sec);
    GetRTCDateInfo(&lm_date, &lm_month, &lm_year);

    last_modified_valid = 1;
}
/* =========================================================
   MENU DISPLAY FUNCTIONS
   ========================================================= */

static void show_main_edit_menu(void)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("1RTC2ON3OF4STS");

    lcd_gotoxy(1, 0);
	if(last_modified_valid)
    {
        
         strlcd("5CL6LG5ELM:");
        lcd_print_2digit_local((u32)lm_hour);
        charlcd(':');
        lcd_print_2digit_local((u32)lm_min);
    }
    else
    {
			//calibration logging
			 strlcd("5CL6LG5ELM:");
        
    }
        
}

static void show_rtc_edit_menu(void)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("1H 2M 3S 4DAY");

    lcd_gotoxy(1, 0);
    strlcd("5DT 6MO 7YR 8EX");
}

/* =========================================================
   RTC EDIT PROCESS
   Updated according to your modified rtc.c:
   SetRTCTimeInfo()
   SetRTCDateInfo()
   SetRTCDay()
   ========================================================= */

static void rtc_edit_process(void)
{
    s32 hour, min, sec;
    s32 date, month, year;
    s32 dow;

    u32 choice;
    s32 value;
    u32 max_date;

    /*
       Read current RTC values first.
    */
    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&dow);

    while(1)
    {
        show_rtc_edit_menu();

        choice = keyscan_timeout(MENU_TIMEOUT_MS);

        if(choice == 0)
        {
            lcd_timeout_message();
            return;
        }

        switch(choice)
        {
            case '1':
                value = read_valid_number("HOUR 0-23", 0, 23);
                 if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                hour = value;
                SetRTCTimeInfo((u32)hour, (u32)min, (u32)sec);
                save_last_modified_time();
						
                event_log(EVENT_RTC_UPDATED);
                lcd_message("HOUR UPDATED", "", 1000);
                break;

            case '2':
                value = read_valid_number("MIN 0-59", 0, 59);
                 if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                min = value;
                SetRTCTimeInfo((u32)hour, (u32)min, (u32)sec);
                save_last_modified_time();
                event_log(EVENT_RTC_UPDATED);
                lcd_message("MIN UPDATED", "", 1000);
                break;

            case '3':
                value = read_valid_number("SEC 0-59", 0, 59);
                 if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                sec = value;
                SetRTCTimeInfo((u32)hour, (u32)min, (u32)sec);
                save_last_modified_time();
                event_log(EVENT_RTC_UPDATED);
                lcd_message("SEC UPDATED", "", 1000);
                break;

            case '4':
                /*
                   Day:
                   0 = SUN
                   1 = MON
                   2 = TUE
                   3 = WED
                   4 = THU
                   5 = FRI
                   6 = SAT
                */
                value = read_valid_number("DAY 0-6", 0, 6);
                if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                dow = value;
                SetRTCDay((u32)dow);
                save_last_modified_time();
                event_log(EVENT_RTC_UPDATED);
                lcd_message("DAY UPDATED", "", 1000);
                break;

            case '5':
                /*
                   Date validation depends on current month and year.
                */
                max_date = days_in_month((u32)month, (u32)year);
                
                if(max_date == 0)
                {
                    lcd_message("MONTH/YEAR ERR", "EDIT FIRST", 1200);
                    break;
                }

                value = read_valid_number("ENTER DATE", 1, (s32)max_date);
                 if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                if(is_valid_date((u32)value, (u32)month, (u32)year))
                {
                    date = value;
                    SetRTCDateInfo((u32)date, (u32)month, (u32)year);
                    save_last_modified_time();
                    event_log(EVENT_RTC_UPDATED);
                    lcd_message("DATE UPDATED", "", 1000);
                }
                else
                {
                    lcd_message("INVALID DATE", "TRY AGAIN", 1200);
                }

                break;

            case '6':
                value = read_valid_number("MONTH 1-12", 1, 12);
                if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                if(is_valid_date((u32)date, (u32)value, (u32)year))
                {
                    month = value;
                    SetRTCDateInfo((u32)date, (u32)month, (u32)year);
                    save_last_modified_time();
                     event_log(EVENT_RTC_UPDATED);
                    lcd_message("MONTH UPDATED", "", 1000);
                }
                else
                {
                    lcd_message("DATE INVALID", "EDIT DATE", 1500);
                }

                break;

            case '7':
                value = read_valid_number("YEAR 0-4095", RTC_YEAR_MIN, RTC_YEAR_MAX);
                 if(value == READNUM_TIMEOUT)
                 {
                    return;
                 }
                if(is_valid_date((u32)date, (u32)month, (u32)value))
                {
                    year = value;
                    SetRTCDateInfo((u32)date, (u32)month, (u32)year);
                    save_last_modified_time();
                    event_log(EVENT_RTC_UPDATED);
                    lcd_message("YEAR UPDATED", "", 1000);
                }
                else
                {
                    lcd_message("DATE INVALID", "EDIT DATE", 1500);
                }

                break;

            case '8':
                lcd_message("RTC UPDATED", "RETURNING...", 1200);
                return;

            default:
                lcd_message("INVALID OPTION", "TRY AGAIN", 1000);
                break;
        }

        /*
           Refresh values after every update.
        */
        GetRTCTimeInfo(&hour, &min, &sec);
        GetRTCDateInfo(&date, &month, &year);
        GetRTCDay(&dow);
    }
}
u8 app_check_admin_password(void)
{
    u8 attempt;
    u8 end_key;
    s32 entered_password;

    for(attempt = 0; attempt < MAX_PASSWORD_TRIES; attempt++)
    {
        lcd_clear();

        lcd_gotoxy(0, 0);
        strlcd("ENTER PASSWORD");

        lcd_gotoxy(1, 0);
        strlcd("PASS:");

        /*
           Enter password and press '='.
        */
        entered_password = readnum(&end_key);

        if((end_key == '=') &&
           (entered_password == ADMIN_PASSWORD))
        {
            lcd_clear();

            lcd_gotoxy(0, 0);
            strlcd("ACCESS GRANTED");

            tdelay_ms(1000);

            return 1;
        }

        lcd_clear();

        lcd_gotoxy(0, 0);
        strlcd("WRONG PASSWORD");

        lcd_gotoxy(1, 0);
        strlcd("TRIES LEFT:");

        u32lcd((u32)(MAX_PASSWORD_TRIES - attempt - 1));

        tdelay_ms(1000);
    }

    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("ACCESS DENIED");

    lcd_gotoxy(1, 0);
    strlcd("MENU LOCKED");

    tdelay_ms(2000);

    return 0;
}
/* =========================================================
   FIRST RTC MENU PROCESS
   ========================================================= */

void app_rtc_menu_process(void)
{
    u32 choice;

    while(1)
    {
        show_main_edit_menu();

        choice = keyscan_timeout(MENU_TIMEOUT_MS);
        if(choice==0)
				{
					lcd_timeout_message();
            return;
				}
        if(choice == '1')
        {
            rtc_edit_process();
            return;
        }
        else if(choice == '2')
        {
					 app_set_on_time_process();
					event_log(EVENT_ON_TIME_UPDATED);
            return;
				}
				else if(choice == '3')
        {
            app_set_off_time_process();
					event_log(EVENT_OFF_TIME_UPDATED);
            return;
        }
				 else if(choice == '4')
        {
            app_status_screen();
            return;
        }
				else if(choice == '5')
				{
            app_calibration_process();
            return;
        }
				else if(choice == '6')
        {
            event_logger_show_history();
            return;
        }
        else
        {
            lcd_message("INVALID OPTION", "TRY AGAIN", 1000);
        }
    }
}

/* =========================================================
   NORMAL APPLICATION TASK
   Updated according to your modified rtc.c.
   ========================================================= */

void app_normal_task(void)
{
    s32 hour, min, sec;
    s32 date, month, year;
    s32 dow;

    /*
       Step 1:
       Read current time, date and day from RTC registers.
    */
    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&dow);

    /*
       Step 2:
       Display RTC information on LCD.
    */
    display_rtc_info(
        (u32)hour,
        (u32)min,
        (u32)sec,
        (u32)date,
        (u32)month,
        (u32)year,
        (u32)dow
    );

    /*
       Step 3:
       Based on hour, control LDR based streetlight.
    */
    streetlight_control( (u32) hour,(u32) min);
}


/////////////////////pwm////////////
void app_display_task(void)
{
    s32 hour, min, sec;
    s32 date, month, year;
    s32 dow;
    u8 brightness;
	
    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);
    //GetRTCDay(&dow);
     /*
       Calculate the weekday automatically from the date.
       Do not use the previously stored RTC weekday.
    */
	 if((month >= 1) &&
       (month <= 12) &&
       (year >= 0) &&
       (year <= 4096) &&
       (date >= 1) &&
       (date <= 31))
    {
    dow = (s32)calculate_day_of_week(
        (u32)date,
        (u32)month,
        (u32)year
    );
    display_rtc_info(
        (u32)hour,
        (u32)min,
        (u32)sec,
        (u32)date,
        (u32)month,
        (u32)year,
        (u32)dow
    );
    brightness = zone_spi_get_display_brightness();

    /*
       Last position of the first LCD row.
    */
    lcd_gotoxy(0, 15);

    if(brightness > 0)
    {
        /*
           CGRAM character 1 = power symbol.
        */
        charlcd(1);
    }
    else
    {
        /*
           Erase power symbol when all lights are OFF.
        */
        charlcd(' ');
    }

    /*
       Show brightness value on LCD line 2.
       Example: B:60
       If this disturbs date display, remove these 4 lines.
    */
    lcd_gotoxy(1, 11);
    strlcd("B:");
u32lcd(zone_spi_get_display_brightness());
    strlcd(" ");
	}
		else
		{
			lcd_clear();
        lcd_gotoxy(0, 0);
        strlcd("RTC DATE INVALID");

        lcd_gotoxy(1, 0);
        strlcd("EDIT RTC FIRST");
    }
}

void app_streetlight_task(void)
{
    s32 hour, min, sec;

    GetRTCTimeInfo(&hour, &min, &sec);

    streetlight_control((u32) hour,(u32) min);
}

void app_keypad_task(void)
{
    /*
       Keep empty for now.
       Do not call blocking keyscan() here.
    */
	
    u8 key;

    key = keyscan_nonblocking();

    if(key == 0)
    {
        return;
    }

    if(key == '+')
    {
        config_set_mode(MODE_AUTO);
        config_storage_save();
        ui_show_message("MODE SELECTED", "AUTO", 800);
    }
    else if(key == '-')
    {
        config_set_mode(MODE_MANUAL_ON);
        config_storage_save();
        ui_show_message("MODE SELECTED", "MANUAL ON", 800);
    }
    else if(key == '/')
    {
        config_set_mode(MODE_MANUAL_OFF);
          config_storage_save();
        ui_show_message("MODE SELECTED", "MANUAL OFF", 800);
    }
}


void app_fault_task(void)
{
    /*
       Future scope:
       LDR fault / LED fault / RTC fault.
    */
	 fault_check();
}
static void app_set_on_time_process(void)
{
    s32 hour;
    s32 min;

    hour = read_valid_number("ON HOUR 0-23", 0, 23);
    min  = read_valid_number("ON MIN 0-59", 0, 59);

    config_set_on_time((u8)hour, (u8)min);
     config_storage_save();
    lcd_message("ON TIME SAVED", "", 1000);
}

static void app_set_off_time_process(void)
{
    s32 hour;
    s32 min;

    hour = read_valid_number("OFF HOUR 0-23", 0, 23);
    min  = read_valid_number("OFF MIN 0-59", 0, 59);

    config_set_off_time((u8)hour, (u8)min);
    config_storage_save();
    lcd_message("OFF TIME SAVED", "", 1000);
}
void app_status_screen(void)
{
    u8 on_h, on_m;
    u8 off_h, off_m;

    config_get_on_time(&on_h, &on_m);
    config_get_off_time(&off_h, &off_m);

    lcd_clear();

    lcd_gotoxy(0, 0);
	   strlcd("Z1:");
    u32lcd(zone_spi_get_brightness(0));
    strlcd(" Z2:");
    u32lcd(zone_spi_get_brightness(1));

    lcd_gotoxy(1, 0);
    strlcd("Z3:");
    u32lcd(zone_spi_get_brightness(2));
    strlcd("Z4:");
    u32lcd(zone_spi_get_brightness(3));
    tdelay_ms(1500);

    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("ON:");
    lcd_print_2digit_local(on_h);
    charlcd(':');
    lcd_print_2digit_local(on_m);

    lcd_gotoxy(1, 0);
    strlcd("OFF:");
    lcd_print_2digit_local(off_h);
    charlcd(':');
    lcd_print_2digit_local(off_m);

    tdelay_ms(1500);

    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("FAULT:");

    lcd_gotoxy(1, 0);
    strlcd(fault_get_text());

    tdelay_ms(1500);

    lcd_clear();
		    tdelay_ms(1500);

    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("MOTION:");

    lcd_gotoxy(1, 0);

    if(motion_is_active())
    {
        strlcd("DETECTED");
    }
    else
    {
        strlcd("NO MOTION");
    }

    tdelay_ms(1500);

    lcd_clear();
}
static void lcd_timeout_message(void)
{
    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("TIME LIMIT OVER");

    lcd_gotoxy(1, 0);
    strlcd("RESUMING...");

    tdelay_ms(1500);

    lcd_clear();
}


static void app_calibration_process(void)
{
    s32 threshold_100;
    s32 threshold_60;
    s32 threshold_30;

    lcd_clear();

    lcd_gotoxy(0, 0);
    strlcd("OLD T100:");
    u32lcd(config_get_threshold_100());

    lcd_gotoxy(1, 0);
    strlcd("T60:");
    u32lcd(config_get_threshold_60());
    strlcd(" T30:");
    u32lcd(config_get_threshold_30());

    tdelay_ms(2000);

    threshold_100 = read_valid_number("T100 0-1023", 0, 1023);

    if(threshold_100 == READNUM_TIMEOUT)
    {
        return;
    }

    threshold_60 = read_valid_number("T60 0-1023", 0, 1023);

    if(threshold_60 == READNUM_TIMEOUT)
    {
        return;
    }

    threshold_30 = read_valid_number("T30 0-1023", 0, 1023);

    if(threshold_30 == READNUM_TIMEOUT)
    {
        return;
    }

    /*
       For current LDR connection:
       darkness = low ADC.
       So threshold order must be:
       T100 < T60 < T30
    */
    if((threshold_100 < threshold_60) && (threshold_60 < threshold_30))
    {
        config_set_ldr_thresholds((u32)threshold_100,
                                  (u32)threshold_60,
                                  (u32)threshold_30);
        config_storage_save();
        event_log(EVENT_CAL_UPDATED);

        lcd_message("CAL UPDATED", "THRESH SAVED", 1200);
    }
    else
    {
        lcd_message("ORDER INVALID", "T100<T60<T30", 1500);
    }
}
void app_zone_spi_light_task(void)
{
    s32 hour, min, sec;

    GetRTCTimeInfo(&hour, &min, &sec);

    zone_spi_control_task((u32)hour, (u32)min);
}

