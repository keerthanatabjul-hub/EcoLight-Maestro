#ifndef RTC_H
#define RTC_H

#include "types.h"
#define RTC_YEAR_MIN    0
#define RTC_YEAR_MAX    4095
/* =========================================================
   RTC TIME STRUCTURE
   ========================================================= */

typedef struct
{
    u32 hour;
    u32 min;
    u32 sec;
    u32 dow;
    u32 date;
    u32 month;
    u32 year;
} RTC_INFO;

/* =========================================================
   DAY DEFINES
   ========================================================= */

#define SUN 0
#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6

/* =========================================================
   RTC FUNCTION DECLARATIONS
   ========================================================= */

void rtc_init(void);

void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second);
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second);

void GetRTCDateInfo(s32 *date, s32 *month, s32 *year);
void SetRTCDateInfo(u32 date, u32 month, u32 year);

void GetRTCDay(s32 *dow);
void SetRTCDay(u32 dow);

#endif
