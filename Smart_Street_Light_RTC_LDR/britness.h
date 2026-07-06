#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "types.h"

#define BRIGHTNESS_OFF       0
#define BRIGHTNESS_LOW       30
#define BRIGHTNESS_MEDIUM    60
#define BRIGHTNESS_HIGH      100

u8 brightness_get_from_adc(u32 adc_value);

#endif