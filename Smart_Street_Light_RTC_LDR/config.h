#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

#define MODE_AUTO        0
#define MODE_MANUAL_ON   1
#define MODE_MANUAL_OFF  2

void config_init(void);

void config_set_mode(u8 mode);
u8 config_get_mode(void);
char *config_get_mode_text(void);

void config_set_on_time(u8 hour, u8 min);
void config_set_off_time(u8 hour, u8 min);

void config_get_on_time(u8 *hour, u8 *min);
void config_get_off_time(u8 *hour, u8 *min);

u8 config_is_light_schedule_active(u8 hour, u8 min);

void config_set_last_adc(u32 adc);
u32 config_get_last_adc(void);

void config_set_brightness(u8 brightness);
u8 config_get_brightness(void);

void config_set_ldr_thresholds(u32 threshold_100, u32 threshold_60, u32 threshold_30);

u32 config_get_threshold_100(void);
u32 config_get_threshold_60(void);
u32 config_get_threshold_30(void);
#endif
