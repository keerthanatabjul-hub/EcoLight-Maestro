//clk defines
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define ADCLK 3000000
#define CLKDIV ((PCLK/ADCLK)-1)
//channel defines
#define CH0 (1<<0)
#define CH1 (1<<1)
#define CH2 (1<<2)
#define CH3  (1<<3)
//SFRs
#define PDN_BIT (1<<21)   
#define CLKDIV_VALUE (CLKDIV<<8)
#define START_CONV (1<<24)

//ADDR defines
#define RESULT 6
#define DONE_BIT 31 
