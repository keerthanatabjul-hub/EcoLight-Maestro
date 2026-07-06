#include <lpc21xx.h>
#include "types.h"
#include "motion.h"

/*
   Motion input pin example:
   P0.15 used as motion sensor input.
   In Proteus, connect push button to this pin.
*/

#define MOTION_PIN        15
#define MOTION_ACTIVE     1

/*
   Motion boost time:
   10 seconds = 100 counts of 100 ms.
*/
#define MOTION_HOLD_COUNT 100

static u8 g_motion_active = 0;
static u16 g_motion_counter = 0;

void motion_init(void)
{
    /*
       P0.15 as input.
    */
    IODIR0 &= ~(1 << MOTION_PIN);

    g_motion_active = 0;
    g_motion_counter = 0;
}

void motion_trigger(void)
{
    g_motion_active = 1;
    g_motion_counter = MOTION_HOLD_COUNT;
}

void motion_task_100ms(void)
{
    /*
       If switch/PIR detects motion, trigger boost.
       Active-low input assumed.
    */
    if(((IOPIN0 >> MOTION_PIN) & 1) == MOTION_ACTIVE)
    {
        motion_trigger();
    }

    if(g_motion_counter > 0)
    {
        g_motion_counter--;

        if(g_motion_counter == 0)
        {
            g_motion_active = 0;
        }
    }
}

u8 motion_is_active(void)
{
    return g_motion_active;
}


