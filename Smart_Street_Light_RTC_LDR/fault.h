#ifndef FAULT_H
#define FAULT_H

#include "types.h"

#define FAULT_NONE              0
#define FAULT_LDR_SENSOR_ERROR  1
#define FAULT_RTC_ERROR         2
#define FAULT_LOW_LIGHT_WARNING 3

void fault_check(void);
u8 fault_get_code(void);
char *fault_get_text(void);

#endif

