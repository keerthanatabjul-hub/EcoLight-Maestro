#include <lpc21xx.h>
#include "types.h"
#include "spi_shift.h"

/*
   Bit-banged SPI for 74HC595

   P0.19 -> DATA / MOSI
   P0.17 -> CLOCK / SCK
   P0.20 -> LATCH / CS
*/

#define SPI_SCK_PIN      17
#define SPI_MOSI_PIN     19
#define SPI_LATCH_PIN    20

#define PINSEL1_BITPOS(pin)   ((pin - 16) * 2)

#define SCK_HIGH()      (IOSET0 = (1UL << SPI_SCK_PIN))
#define SCK_LOW()       (IOCLR0 = (1UL << SPI_SCK_PIN))

#define MOSI_HIGH()     (IOSET0 = (1UL << SPI_MOSI_PIN))
#define MOSI_LOW()      (IOCLR0 = (1UL << SPI_MOSI_PIN))

#define LATCH_HIGH()    (IOSET0 = (1UL << SPI_LATCH_PIN))
#define LATCH_LOW()     (IOCLR0 = (1UL << SPI_LATCH_PIN))

void spi_shift_init(void)
{
    /*
       Configure P0.17, P0.19, P0.20 as GPIO.
    */
    PINSEL1 &= ~(3UL << PINSEL1_BITPOS(SPI_SCK_PIN));
    PINSEL1 &= ~(3UL << PINSEL1_BITPOS(SPI_MOSI_PIN));
    PINSEL1 &= ~(3UL << PINSEL1_BITPOS(SPI_LATCH_PIN));

    /*
       Configure as output.
    */
    IODIR0 |= (1UL << SPI_SCK_PIN);
    IODIR0 |= (1UL << SPI_MOSI_PIN);
    IODIR0 |= (1UL << SPI_LATCH_PIN);

    SCK_LOW();
    MOSI_LOW();
    LATCH_LOW();

    spi_shift_write_byte(0x00);
}

void spi_shift_write_byte(u8 data)
{
    s32 bitpos;

    /*
       Keep clock and latch LOW before transmission.
    */
    SCK_LOW();
    LATCH_LOW();
    /*
       Send MSB first to 74HC595.
    */
    for(bitpos = 7; bitpos >= 0; bitpos--)
    {
        if(data & (1 << bitpos))
        {
            MOSI_HIGH();
        }
        else
        {
            MOSI_LOW();
        }

        /*
           Rising edge shifts one bit into 74HC595.
        */

        SCK_HIGH();
        
        SCK_LOW();
       
    }

    /*
       Latch output.
    */
    LATCH_HIGH();
    
    LATCH_LOW();
}


