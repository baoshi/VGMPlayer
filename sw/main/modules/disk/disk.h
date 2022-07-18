#pragma once

#include <stdbool.h>
#include <ff.h>

#ifdef __cplusplus
extern "C" {
#endif

void disk_init();
int disk_card_detect(uint32_t now);
int  disk_check_dir(const char* dir);
bool disk_present();
const char* disk_result_str(FRESULT fr);

#ifdef __cplusplus
}
#endif
