#ifndef SOFT_UART_H
#define SOFT_UART_H

#include "types.h"

void soft_uart_init(void);
void soft_uart_TX(u8 sdat);
u8 soft_uart_RX(void);

void soft_uart_str(char *str);
void soft_uart_u32(u32 num);
void soft_uart_crlf(void);

#endif


