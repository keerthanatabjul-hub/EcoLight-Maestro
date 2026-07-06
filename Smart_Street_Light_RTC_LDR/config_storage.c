#include "types.h"
#include "eeprom.h"
#include "config.h"
#include "config_storage.h"

#define EEPROM_MAGIC_ADDR      0
#define EEPROM_MAGIC_VALUE     0xA5

#define EEPROM_ON_HOUR_ADDR    1
#define EEPROM_ON_MIN_ADDR     2
#define EEPROM_OFF_HOUR_ADDR   3
#define EEPROM_OFF_MIN_ADDR    4
#define EEPROM_MODE_ADDR       5

#define EEPROM_T100_ADDR       6
#define EEPROM_T60_ADDR        8
#define EEPROM_T30_ADDR        10

#define EEPROM_CHECKSUM_ADDR   12

static u8 config_storage_checksum(void)
{
    u8 sum = 0;
    u8 i;

    for(i = 1; i < EEPROM_CHECKSUM_ADDR; i++)
    {
        sum = sum + eeprom_read_byte(i);
    }

    return sum;
}

void config_storage_save(void)
{
    u8 on_h, on_m;
    u8 off_h, off_m;
    u8 checksum;

    config_get_on_time(&on_h, &on_m);
    config_get_off_time(&off_h, &off_m);

    eeprom_write_byte(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);

    eeprom_write_byte(EEPROM_ON_HOUR_ADDR, on_h);
    eeprom_write_byte(EEPROM_ON_MIN_ADDR, on_m);

    eeprom_write_byte(EEPROM_OFF_HOUR_ADDR, off_h);
    eeprom_write_byte(EEPROM_OFF_MIN_ADDR, off_m);

    eeprom_write_byte(EEPROM_MODE_ADDR, config_get_mode());

    eeprom_write_u16(EEPROM_T100_ADDR, config_get_threshold_100());
    eeprom_write_u16(EEPROM_T60_ADDR,  config_get_threshold_60());
    eeprom_write_u16(EEPROM_T30_ADDR,  config_get_threshold_30());

    checksum = config_storage_checksum();

    eeprom_write_byte(EEPROM_CHECKSUM_ADDR, checksum);
}

void config_storage_load(void)
{
    u8 magic;
    u8 stored_checksum;
    u8 calculated_checksum;

    u8 on_h, on_m;
    u8 off_h, off_m;
    u8 mode;

    u32 t100;
    u32 t60;
    u32 t30;

    magic = eeprom_read_byte(EEPROM_MAGIC_ADDR);

    if(magic != EEPROM_MAGIC_VALUE)
    {
        /*
           EEPROM not initialized.
           Keep default config values.
        */
        config_storage_save();
        return;
    }

    stored_checksum = eeprom_read_byte(EEPROM_CHECKSUM_ADDR);
    calculated_checksum = config_storage_checksum();

    if(stored_checksum != calculated_checksum)
    {
        /*
           EEPROM data corrupted.
           Keep default config values.
        */
        config_storage_save();
        return;
    }

    on_h  = eeprom_read_byte(EEPROM_ON_HOUR_ADDR);
    on_m  = eeprom_read_byte(EEPROM_ON_MIN_ADDR);
    off_h = eeprom_read_byte(EEPROM_OFF_HOUR_ADDR);
    off_m = eeprom_read_byte(EEPROM_OFF_MIN_ADDR);

    mode = eeprom_read_byte(EEPROM_MODE_ADDR);

    t100 = eeprom_read_u16(EEPROM_T100_ADDR);
    t60  = eeprom_read_u16(EEPROM_T60_ADDR);
    t30  = eeprom_read_u16(EEPROM_T30_ADDR);

    config_set_on_time(on_h, on_m);
    config_set_off_time(off_h, off_m);
    config_set_mode(mode);
    config_set_ldr_thresholds(t100, t60, t30);
}



