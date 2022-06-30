#pragma once

#include <hardware/flash.h>

enum
{
    EEPROM_OK = 0,
    EEPROM_NOT_INITIALIZED = 1,

};

/*
 * Simulated EEPROM using FLASH
 * EEPROM region is defined in the link script
 * Exposed variable __eeprom_pool_start is the starting address
  * Note flash.h defines 
 *   FLASH_SECTOR_SIZE = 4096 which is the minimal erase size
 *   FLASH_PAGE_SIZE = 256 which is the minimal write size
 */

#define EEPROM_POOL_SIZE 4096                                               // 4096 bytes used to simulate EEPROM, must be in sync with rp2040_16m.ld

#define EEPROM_POOL_WRITE_OFFS  (PICO_FLASH_SIZE_BYTES - EEPROM_POOL_SIZE)  // Offset of EEPROM pool data

extern uint8_t __eeprom_pool_start;
#define EEPROM_POOL_READ_ADDR   ((uint32_t)(&__eeprom_pool_start))          // Memory mapped address to read EEPROM data pool


#define EEPROM_SIZE 64    // Simulated EEPROM size

#define EEPROM_CELLS_PER_PAGE       (FLASH_PAGE_SIZE / EEPROM_SIZE)         // # of EEPROM cells in one page (must write together)
#define EEPROM_CELLS_PER_SECTOR     (FLASH_SECTOR_SIZE / EEPROM_SIZE)       // # of EEPROM cells in one sector (must erase together)
#define EEPROM_CELLS                (EEPROM_POOL_SIZE / EEPROM_SIZE)        // Total EEPROM cells
#define EEPROM_PAGES                (EEPROM_POOL_SIZE / FLASH_PAGE_SIZE)
#define EEPROM_SECTORS              (EEPROM_POOL_SIZE / FLASH_SECTOR_SIZE)


void eeprom_erase_all();
void eeprom_write(uint8_t *data);
int eeprom_read(uint8_t *data);




