#include "types.h"
#include "soft_uart.h"
#include "rtc.h"
#include "config.h"
#include "fault.h"
#include "zone_light_spi.h"
#include "alarm.h"
#include "uart_monitor.h"
static void uart_print_2digit(u32 num)
{
    soft_uart_TX((num / 10) + '0');
    soft_uart_TX((num % 10) + '0');
}

void uart_monitor_send_status(void)
{
    s32 hour, min, sec;
    s32 date, month, year;
    u8 on_h, on_m;
    u8 off_h, off_m;

    GetRTCTimeInfo(&hour, &min, &sec);
    GetRTCDateInfo(&date, &month, &year);

    config_get_on_time(&on_h, &on_m);
    config_get_off_time(&off_h, &off_m);

    soft_uart_str("========== ECOLIGHT STATUS ==========");
    soft_uart_crlf();

    soft_uart_str("TIME: ");
    uart_print_2digit((u32)hour);
    soft_uart_TX(':');
    uart_print_2digit((u32)min);
    soft_uart_TX(':');
    uart_print_2digit((u32)sec);
    soft_uart_crlf();

    soft_uart_str("DATE: ");
    uart_print_2digit((u32)date);
    soft_uart_TX('/');
    uart_print_2digit((u32)month);
    soft_uart_TX('/');
    soft_uart_u32((u32)year);
    soft_uart_crlf();

    soft_uart_str("MODE: ");
    soft_uart_str(config_get_mode_text());
    soft_uart_crlf();

    soft_uart_str("ON TIME: ");
    uart_print_2digit(on_h);
    soft_uart_TX(':');
    uart_print_2digit(on_m);
    soft_uart_crlf();

    soft_uart_str("OFF TIME: ");
    uart_print_2digit(off_h);
    soft_uart_TX(':');
    uart_print_2digit(off_m);
    soft_uart_crlf();

    soft_uart_str("ADC: ");
    soft_uart_u32(config_get_last_adc());
    soft_uart_crlf();

    soft_uart_str("THRESHOLDS: ");
    soft_uart_str("T100=");
    soft_uart_u32(config_get_threshold_100());
    soft_uart_str(" T60=");
    soft_uart_u32(config_get_threshold_60());
    soft_uart_str(" T30=");
    soft_uart_u32(config_get_threshold_30());
    soft_uart_crlf();

    soft_uart_str("DISPLAY B: ");
    soft_uart_u32(zone_spi_get_display_brightness());
    soft_uart_TX('%');
    soft_uart_crlf();

    soft_uart_str("ZONE BRIGHTNESS: ");
    soft_uart_str("Z1=");
    soft_uart_u32(zone_spi_get_brightness(0));
    soft_uart_TX('%');

    soft_uart_str(" Z2=");
    soft_uart_u32(zone_spi_get_brightness(1));
    soft_uart_TX('%');

    soft_uart_str(" Z3=");
    soft_uart_u32(zone_spi_get_brightness(2));
    soft_uart_TX('%');

    soft_uart_str(" Z4=");
    soft_uart_u32(zone_spi_get_brightness(3));
    soft_uart_TX('%');
    soft_uart_crlf();

    soft_uart_str("MOTION: ");
    soft_uart_str("Z1=");
    soft_uart_u32(zone_spi_motion_is_active(0));

    soft_uart_str(" Z2=");
    soft_uart_u32(zone_spi_motion_is_active(1));

    soft_uart_str(" Z3=");
    soft_uart_u32(zone_spi_motion_is_active(2));

    soft_uart_str(" Z4=");
    soft_uart_u32(zone_spi_motion_is_active(3));
    soft_uart_crlf();

    soft_uart_str("FAULT: ");
    soft_uart_str(fault_get_text());
    soft_uart_crlf();

    soft_uart_str("ALARM: ");
    if(alarm_is_active())
    {
        soft_uart_str("ACTIVE");
    }
    else
    {
        soft_uart_str("IDLE");
    }
    soft_uart_crlf();

    soft_uart_str("=====================================");
    soft_uart_crlf();
    soft_uart_crlf();
}

