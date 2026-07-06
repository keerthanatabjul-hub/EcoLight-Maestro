#ifndef I2C_DEFINES_H
#define I2C_DEFINES_H

#define SCL        21    /* P0.2 */
#define SDA        22   /* P0.3 */

#define I2EN_BIT   6
#define STA_BIT    5
#define STO_BIT    4
#define SI_BIT     3
#define AA_BIT     2

#define I2ENC_BIT  6
#define STAC_BIT   5
#define SIC_BIT    3
#define AAC_BIT    2

/*
   For PCLK = 15 MHz and I2C = 100 kHz:
   SCL = PCLK / (I2SCLH + I2SCLL)
   100k = 15MHz / (75 + 75)
*/
#define LOADVAL    75

#endif

