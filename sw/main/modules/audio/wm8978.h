#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    WM8978_OUTPUT_SPEAKER = 0,
    WM8978_OUTPUT_EARPIECE
} wm8978_output_t;


void wm8978_init();
void wm8978_powerup_stage1();
void wm8978_powerup_stage2();
void wm8978_powerdown();
void wm8978_set_volume(wm8978_output_t chn, uint8_t vol);



#ifdef __cplusplus
}
#endif