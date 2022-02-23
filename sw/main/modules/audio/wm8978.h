#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>



typedef enum
{
    WM8978_OUTPUT_SPEAKER = 0,
    WM8978_OUTPUT_EARPIECE
} wm8978_output_t;


void wm8978_preinit();
void wm8978_postinit();
void wm8978_powerdown();
void wm8978_set_volume(wm8978_output_t chn, uint8_t vol);



#ifdef __cplusplus
}
#endif