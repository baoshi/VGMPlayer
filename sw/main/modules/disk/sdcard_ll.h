#pragma once

#include <stdbool.h>
#include <stdint.h>


// This matches diskio.h in FatFs
#ifndef _DISKIO_DEFINED
typedef unsigned char DSTATUS;
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#endif

// SD/MMC card types
#define SDCARD_NONE     0   // No card is present
#define SDCARD_V1       1   // v1.x Standard Capacity
#define SDCARD_V2       2   // v2.x Standard capacity SD card
#define SDCARD_V2HC     3   // v2.x High capacity SD card
#define CARD_UNKNOWN    4   // Unknown or unsupported card


#define SD_BLOCK_SIZE   512 // Block size supported for SD card is 512 bytes

#define SDCARD_ERROR_OK              0
#define SDCARD_ERROR_NO_DEVICE      -1      // No card present or card is not responding to SPI command
#define SDCARD_ERROR_NO_INIT        -2      // Card detected but not initialized (sdcard_init() not called)
#define SDCARD_ERROR_UNUSABLE       -3      // CMD8 fail for V2 card or OCR returns no 3.3V support
#define SDCARD_ERROR_UNSUPPORTED    -4      // Unsupported SPI command
#define SDCARD_ERROR_PARAMETER      -5      // Parameter error from either our check or card return
#define SDCARD_ERROR_CRC            -6      // CRC error
#define SDCARD_ERROR_NO_RESPONSE    -7      // Timeout
#define SDCARD_ERROR_WRITE          -8      // SPI write block error
#define SDCARD_ERROR_ERASE          -9      // R1 ERASE_RESET or ERASE_SQEUENCE_ERROR
#define SDCARD_ERROR_UNKNOWN        -99

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*card_detect_cb_t)(bool inserted, void* param);
void sdcard_ll_init(card_detect_cb_t callback, void* param);

void sdcard_ll_task(uint32_t now);

bool sdcard_ll_card_detected();

#ifdef __cplusplus
}
#endif
