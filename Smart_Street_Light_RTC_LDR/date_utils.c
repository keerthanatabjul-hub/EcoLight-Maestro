#include "types.h"
#include "date_utils.h"

u8 is_leap_year(u32 year)
{
    if((year % 400) == 0)
        return 1;
    else if((year % 100) == 0)
        return 0;
    else if((year % 4) == 0)
        return 1;
    else
        return 0;
}

u32 days_in_month(u32 month, u32 year)
{
    switch(month)
    {
        case 1:  return 31;
        case 2:  return is_leap_year(year) ? 29 : 28;
        case 3:  return 31;
        case 4:  return 30;
        case 5:  return 31;
        case 6:  return 30;
        case 7:  return 31;
        case 8:  return 31;
        case 9:  return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;
        default: return 0;
    }
}

u8 is_valid_date(u32 date, u32 month, u32 year)
{
    u32 max_days;

    if(year > 4095)
        return 0;

    if(month < 1 || month > 12)
        return 0;

    max_days = days_in_month(month, year);

    if(date < 1 || date > max_days)
        return 0;

    return 1;
}
u8 calculate_day_of_week(u32 date, u32 month, u32 year)
{
    static const u8 month_table[12] =
    {
        0, 3, 2, 5, 0, 3,
        5, 1, 4, 6, 2, 4
    };

    /*
       Return value:
       0 = Sunday
       1 = Monday
       2 = Tuesday
       3 = Wednesday
       4 = Thursday
       5 = Friday
       6 = Saturday
    */
     if((month == 0) || (month > 12))
    {
        return 0;
    }

    if(month < 3)
    {
        year--;
    }

    return (u8)
    (
        (year +
         year / 4 -
         year / 100 +
         year / 400 +
         month_table[month - 1] +
         date) % 7
    );
}