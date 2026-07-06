#include <lpc21xx.h>
#include "types.h"

#include "rtc.h"

/* =========================================================
   SYSTEM CLOCK AND PERIPHERAL CLOCK MACROS
   ========================================================= */

#define FOSC    12000000UL
#define CCLK    (5 * FOSC)
#define PCLK    (CCLK / 4)

/*
   RTC requires 32.768 kHz clock for 1-second increment.

   PREINT  = int(PCLK / 32768) - 1
   PREFRAC = PCLK - ((PREINT + 1) * 32768)
*/

#define PREINT_VAL     ((PCLK / 32768) - 1)
#define PREFRAC_VAL    (PCLK - ((PREINT_VAL + 1) * 32768))

/* =========================================================
   RTC CONTROL REGISTER BITS
   ========================================================= */

#define RTC_ENABLE     (1 << 0)
#define RTC_RESET      (1 << 1)

/*
   For LPC2148:
   CCR bit 4 selects clock source.

   0 = PCLK based RTC clock
   1 = External 32.768 kHz RTC oscillator

   In your sample code, PCLK based calculation is used.
   So RTC_CLKSRC is not enabled here.
*/
#define RTC_CLKSRC     (1 << 4)

/* =========================================================
   RTC INITIALIZATION
   ========================================================= */

void rtc_init(void)
{
    /*
       Step 1:
       Reset RTC counters.
    */
    CCR = RTC_RESET;

    /*
       Step 2:
       Load prescaler values.
       This generates RTC 1-second timing from PCLK.
    */
    PREINT = PREINT_VAL;
    PREFRAC = PREFRAC_VAL;

    /*
       Step 3:
       Enable RTC.
    */
    CCR = RTC_ENABLE;
}

void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
    *hour = HOUR;
    *minute = MIN;
    *second = SEC;
}

void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
    HOUR = hour;
    MIN = minute;
    SEC = second;
}

void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
    *date = DOM;
    *month = MONTH;
    *year = YEAR;
}

void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
    DOM = date;
    MONTH = month;
    YEAR = year;
}

void GetRTCDay(s32 *dow)
{
    *dow = DOW;
}

void SetRTCDay(u32 dow)
{
    DOW = dow;
}

