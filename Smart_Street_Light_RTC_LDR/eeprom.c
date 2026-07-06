#include "types.h"
#include "delay.h"
#include "i2c.h"
#include "eeprom.h"

/*
   AT24C1024 = 128 KB EEPROM.

   Address range:
   0x00000 to 0x1FFFF

   Lower 64 KB  -> device address 0xA0 / 0xA1
   Upper 64 KB  -> device address 0xA2 / 0xA3
*/

#define EEPROM_BASE_ADDR   0xA0

static u8 eeprom_get_sla_w(u32 mem_addr)
{
    u8 block;

    /*
       bit16 selects lower/upper 64 KB block.
       block = 0 for 0x00000 - 0x0FFFF
       block = 1 for 0x10000 - 0x1FFFF
    */
    block = (mem_addr >> 16) & 1;

    return EEPROM_BASE_ADDR | (block << 1);
}

static u8 eeprom_get_sla_r(u32 mem_addr)
{
    return eeprom_get_sla_w(mem_addr) | 1;
}

void eeprom_write_byte(u32 mem_addr, u8 data)
{
    u8 sla_w;

    sla_w = eeprom_get_sla_w(mem_addr);

    i2c_start();

    i2c_write(sla_w);

    /*
       Send lower 16-bit memory address.
    */
    i2c_write((u8)((mem_addr >> 8) & 0xFF));
    i2c_write((u8)(mem_addr & 0xFF));

    i2c_write(data);

    i2c_stop();

    /*
       EEPROM internal write cycle delay.
    */
    delay_ms(10);
}

u8 eeprom_read_byte(u32 mem_addr)
{
    u8 data;
    u8 sla_w;
    u8 sla_r;

    sla_w = eeprom_get_sla_w(mem_addr);
    sla_r = eeprom_get_sla_r(mem_addr);

    i2c_start();

    i2c_write(sla_w);

    /*
       Send lower 16-bit memory address.
    */
    i2c_write((u8)((mem_addr >> 8) & 0xFF));
    i2c_write((u8)(mem_addr & 0xFF));

    i2c_restart();

    i2c_write(sla_r);

    data = i2c_read_nack();

    i2c_stop();

    return data;
}

void eeprom_write_u16(u32 mem_addr, u32 value)
{
    eeprom_write_byte(mem_addr,     (u8)(value & 0xFF));
    eeprom_write_byte(mem_addr + 1, (u8)((value >> 8) & 0xFF));
}

u32 eeprom_read_u16(u32 mem_addr)
{
    u32 low;
    u32 high;

    low  = eeprom_read_byte(mem_addr);
    high = eeprom_read_byte(mem_addr + 1);

    return low | (high << 8);
}

