#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "types.h"

void ui_manager_init(void);
void ui_show_message(char *line1, char *line2, u32 duration_ms);
void ui_task_100ms(void);
u8 ui_is_message_active(void);

#endif

