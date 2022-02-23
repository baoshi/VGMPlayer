#include "wm8978.h"
#include "audio.h"

void audio_init()
{

}


void audio_powerup_stage1()
{
    wm8978_preinit();
}


void audio_powerup_stage2()
{
    wm8978_postinit();
}


void audio_powerdown()
{
}


/**
 * @brief Call this function repeatedly to receive jack detection notification
 * 
 * @param now       Timestamp
 * @return * int    1 if earpiece plugged
 *                  2 if earpiece unplugged
 *                  0 if no change
 */
int audio_update(uint32_t now)
{
    return 0;
}
