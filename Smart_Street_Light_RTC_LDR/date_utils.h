#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include "types.h"

u8 is_leap_year(u32 year);
u32 days_in_month(u32 month, u32 year);
u8 is_valid_date(u32 date, u32 month, u32 year);
u8 calculate_day_of_week(u32 date, u32 month, u32 year);
#endif
