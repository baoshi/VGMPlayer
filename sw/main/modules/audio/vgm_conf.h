#pragma once

#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))

#include "audio.h"
#define VGM_PRINTINF(...) AUD_LOGI(__VA_ARGS__)
#define VGM_PRINTDBG(...) AUD_LOGD(__VA_ARGS__)
#define VGM_PRINTERR(...) AUD_LOGE(__VA_ARGS__)
#define VGM_ASSERT MY_ASSERT
#define VGM_MALLOC audio_malloc
#define VGM_FREE audio_free

#define VGM_FILE_CACHE_SIZE     2048

#define NESAPU_USE_BLIPBUF      1
#define NESAPU_MAX_SAMPLES      2048
#define NESAPU_RAM_CACHE_SIZE   4096
