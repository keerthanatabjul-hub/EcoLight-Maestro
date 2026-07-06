#include "types.h"
void init_kpm(void);
u32 colscan(void);
u32 rowcheck(void);
u32 colcheck(void);
u32 keyscan(void);
s32 readnum(u8 *end_key);

s32 readnum_timeout_digits(u8 *end_key, u32 timeout_ms, u8 max_digits);
s32 readnum_timeout(u8 *end_key, u32 timeout_ms);
u8 keyscan_timeout(u32 timeout_ms);
u8 keyscan_nonblocking(void);
#define READNUM_NO_INPUT   -9999
#define READNUM_TIMEOUT    -9998
#define READNUM_DIGIT_LIMIT   -9997

