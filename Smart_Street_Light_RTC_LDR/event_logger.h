#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include "types.h"

#define EVENT_SYSTEM_START        1
#define EVENT_MODE_AUTO           2
#define EVENT_MODE_MANUAL_ON      3
#define EVENT_MODE_MANUAL_OFF     4
#define EVENT_RTC_UPDATED         5
#define EVENT_ON_TIME_UPDATED     6
#define EVENT_OFF_TIME_UPDATED    7
#define EVENT_CAL_UPDATED         8
#define EVENT_BRIGHTNESS_CHANGED  9
#define EVENT_MOTION_DETECTED     10
#define EVENT_ALARM_ON            11
#define EVENT_ALARM_OFF           12
#define EVENT_FAULT_DETECTED      13

void event_logger_init(void);
void event_log(u8 event_code);
void event_logger_show_history(void);
char *event_get_text(u8 event_code);

#endif

