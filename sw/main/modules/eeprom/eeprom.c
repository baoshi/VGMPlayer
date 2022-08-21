#include <string.h>
#include <pico.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <pico/time.h>
#include <hardware/sync.h>
#include "hw_conf.h"
#include "my_debug.h"
#include "eeprom.h"

#ifndef EEPROM_DEBUG
# define EEPROM_DEBUG 1
#endif


// Debug log
#if EEPROM_DEBUG
#define EE_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define EE_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define EE_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define EE_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define EE_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define EE_LOGD(x, ...)
#define EE_LOGI(x, ...)
#define EE_LOGW(x, ...)
#define EE_LOGE(x, ...)
#define EE_DEBUGF(x, ...)
#endif


static int _cur_cell = -1;  // use to track last active cell


static void _erase_sector(uint8_t sector)
{
    // No interrupts during FLASH writing
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(EEPROM_POOL_WRITE_OFFS + (sector * FLASH_SECTOR_SIZE), FLASH_SECTOR_SIZE);
    restore_interrupts (ints);
}


// Read cell data
static void _read_cell(int cell, uint8_t *data)
{
    uint32_t addr = EEPROM_POOL_READ_ADDR + cell * EEPROM_SIZE;
    memcpy(data, (const void *)addr, EEPROM_SIZE);
}


// Find the index of the active cell of EEPROM
// active cell: contains valid data, while next cell is all FF
// Algorithm: Starting the end, find first non-empty cell
// Note: Hacky: We determine if a cell is empty by check if the first and last bytes are both 0xFF
static int _locate_active_cell(uint8_t *out)
{
    int cell;
    uint8_t data[EEPROM_SIZE];
    for (cell = EEPROM_CELLS - 1; cell >= 0; --cell)
    {
        _read_cell(cell, data);
        if ((data[0] != 0xFF) || (data[EEPROM_SIZE - 1] != 0xFF))
        {
            if (out) memcpy(out, data, EEPROM_SIZE);
            _cur_cell = cell;
            break;
        }
    }
    return cell;
}


void eeprom_erase_all()
{
    for (uint8_t i = 0; i < EEPROM_SECTORS; ++i)
        _erase_sector(i);
    _cur_cell = -1;
}


int eeprom_read(uint8_t *data)
{
    int r = EEPROM_OK;
    int cell;
    if (_cur_cell >= 0)
    {
        cell = _cur_cell;
        EE_LOGD("EEPROM: Read from cell %d\n", cell);
        _read_cell(cell, data);
    }
    else
    {
        cell = _locate_active_cell(data);
        if (cell < 0)
        {
            EE_LOGD("EEPROM: Uninitialized FLASH\n");
            r = EEPROM_NOT_INITIALIZED;
        }
        else
        {
            EE_LOGD("EEPROM: Read from cell %d\n", cell);
        }
    }
    return r;
}



// Write algorithm:
// 1. Find which cell to write
// 2. If cell >= EEPROM_CELLS, erase everyting and start from cell 0
// 3. Prepare a new page, read cells up to current one, append new cell data and write
void eeprom_write(uint8_t *data)
{
    int cell = _cur_cell;
    uint8_t pd[FLASH_PAGE_SIZE];
    if (cell < 0)
    {
        cell = _locate_active_cell(0);
        if (cell < 0)
        {
            // Flash all empty, start write from 0
            cell = 0;
        }
    }
    else
    {
        ++cell;
    }
    if (cell >= EEPROM_CELLS)
    {
        EE_LOGD("EEPROM: Erase EEPROM sector(s)\n");
        eeprom_erase_all();
        cell = 0;
    }
    memset(pd, 0xFF, FLASH_PAGE_SIZE);
    int page = cell / EEPROM_CELLS_PER_PAGE;
    int cell_in_page = cell % EEPROM_CELLS_PER_PAGE;
    // Read back data from the same page
    for (int i = 0; i < cell_in_page; ++i)
    {
        EE_LOGD("Load page from cell %d\n", page * EEPROM_CELLS_PER_PAGE + i);
        _read_cell(page * EEPROM_CELLS_PER_PAGE + i, pd + i * EEPROM_SIZE);
    }
    // copy new page
    memcpy(pd + cell_in_page * EEPROM_SIZE, data, EEPROM_SIZE);
    EE_LOGD("EEPROM: Write to cell %d (%d of page %d)\n", cell, cell_in_page, page);
    // write data
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(EEPROM_POOL_WRITE_OFFS + (page * FLASH_PAGE_SIZE), pd, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
    _cur_cell = cell;
}