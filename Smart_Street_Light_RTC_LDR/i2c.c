#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "i2c.h"

/*
   Software I2C / Bit-banging I2C

   P0.21 -> SCL
   P0.22 -> SDA

   External 4.7k pull-up resistors are required on SCL and SDA.
*/

#define SOFT_I2C_SCL   21
#define SOFT_I2C_SDA   22

#define I2C_DELAY_US   5

#define SCL_LOW()      do { IODIR0 |=  (1UL << SOFT_I2C_SCL); IOCLR0 = (1UL << SOFT_I2C_SCL); } while(0)
#define SCL_RELEASE()  do { IODIR0 &= ~(1UL << SOFT_I2C_SCL); } while(0)

#define SDA_LOW()      do { IODIR0 |=  (1UL << SOFT_I2C_SDA); IOCLR0 = (1UL << SOFT_I2C_SDA); } while(0)
#define SDA_RELEASE()  do { IODIR0 &= ~(1UL << SOFT_I2C_SDA); } while(0)

#define SDA_READ()     ((IOPIN0 >> SOFT_I2C_SDA) & 1)

static void i2c_delay(void)
{
    delay_us(I2C_DELAY_US);
}

void Init_I2C(void)
{
    /*
       Configure P0.21 and P0.22 as GPIO.
       P0.21 uses PINSEL1 bits 11:10
       P0.22 uses PINSEL1 bits 13:12
    */
    PINSEL1 &= ~(3UL << ((SOFT_I2C_SCL - 16) * 2));
    PINSEL1 &= ~(3UL << ((SOFT_I2C_SDA - 16) * 2));

    /*
       Release both lines.
       Pull-up resistors will make both lines HIGH.
    */
    SCL_RELEASE();
    SDA_RELEASE();

    i2c_delay();
}

void i2c_start(void)
{
    SDA_RELEASE();
    SCL_RELEASE();
    i2c_delay();

    SDA_LOW();
    i2c_delay();

    SCL_LOW();
    i2c_delay();
}

void i2c_restart(void)
{
    i2c_start();
}

void i2c_stop(void)
{
    SDA_LOW();
    i2c_delay();

    SCL_RELEASE();
    i2c_delay();

    SDA_RELEASE();
    i2c_delay();
}

void i2c_write(u8 dat)
{
    s32 i;
    u8 ack;

    for(i = 7; i >= 0; i--)
    {
        if(dat & (1 << i))
        {
            SDA_RELEASE();   /* send 1 */
        }
        else
        {
            SDA_LOW();       /* send 0 */
        }

        i2c_delay();

        SCL_RELEASE();
        i2c_delay();

        SCL_LOW();
        i2c_delay();
    }

    /*
       ACK bit from slave.
       Release SDA and read during 9th clock.
    */
    SDA_RELEASE();
    i2c_delay();

    SCL_RELEASE();
    i2c_delay();

    ack = SDA_READ();

    SCL_LOW();
    i2c_delay();

    /*
       ack = 0 means slave acknowledged.
       We are not returning it now because your old function was void.
    */
    (void)ack;
}

u8 i2c_read_ack(void)
{
    u8 i;
    u8 dat = 0;

    SDA_RELEASE();

    for(i = 0; i < 8; i++)
    {
        dat <<= 1;

        SCL_RELEASE();
        i2c_delay();

        if(SDA_READ())
        {
            dat |= 1;
        }

        SCL_LOW();
        i2c_delay();
    }

    /*
       Send ACK = 0
    */
    SDA_LOW();
    i2c_delay();

    SCL_RELEASE();
    i2c_delay();

    SCL_LOW();
    i2c_delay();

    SDA_RELEASE();

    return dat;
}

u8 i2c_read_nack(void)
{
    u8 i;
    u8 dat = 0;

    SDA_RELEASE();

    for(i = 0; i < 8; i++)
    {
        dat <<= 1;

        SCL_RELEASE();
        i2c_delay();

        if(SDA_READ())
        {
            dat |= 1;
        }

        SCL_LOW();
        i2c_delay();
    }

    /*
       Send NACK = 1
    */
    SDA_RELEASE();
    i2c_delay();

    SCL_RELEASE();
    i2c_delay();

    SCL_LOW();
    i2c_delay();

    return dat;
}


