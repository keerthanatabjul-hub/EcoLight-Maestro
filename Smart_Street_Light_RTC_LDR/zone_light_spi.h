#ifndef ZONE_LIGHT_SPI_H
#define ZONE_LIGHT_SPI_H

#include "types.h"

#define ZONE_COUNT 4

void zone_spi_light_init(void);
void zone_spi_pwm_task_100us(void);

void zone_spi_motion_task_100ms(void);
void zone_spi_control_task(u32 hour, u32 min);

void zone_spi_set_brightness(u8 zone, u8 brightness);
u8 zone_spi_get_brightness(u8 zone);
u8 zone_spi_motion_is_active(u8 zone);
u8 zone_spi_get_display_brightness(void);

#endif

