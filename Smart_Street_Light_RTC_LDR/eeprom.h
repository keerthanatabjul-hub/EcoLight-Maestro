#ifndef EEPROM_H
#define EEPROM_H

#include "types.h"

void eeprom_write_byte(u32 mem_addr, u8 data);
u8 eeprom_read_byte(u32 mem_addr);

void eeprom_write_u16(u32 mem_addr, u32 value);
u32 eeprom_read_u16(u32 mem_addr);

#endif
