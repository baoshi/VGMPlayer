/* I/O module for FatFs */

#include <ff.h>
#include <diskio.h>
#include "hw_conf.h"
#include "my_debug.h"
#include "sdcard_ll.h"


#ifndef SDCARD_FATFS_IO_DEBUG
# define SDCARD_FATFS_IO_DEBUG 0
#endif


// Debug log
#if SDCARD_FATFS_IO_DEBUG
#define SF_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define SF_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define SF_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define SF_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define SF_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define SF_LOGD(x, ...)
#define SF_LOGW(x, ...)
#define SF_LOGE(x, ...)
#define SF_LOGI(x, ...)
#define SF_DEBUGF(x, ...)
#endif


#define SDCARD_DRV_NUM 0

extern int sdcard_init();
extern int sdcard_read_blocks(uint8_t *buffer, uint64_t ulSectorNumber, uint32_t ulSectorCount);
extern int sdcard_write_blocks(const uint8_t *buffer, uint64_t ulSectorNumber, uint32_t blockCnt);
extern int sdcard_get_dstatus();
extern int64_t sdcard_get_sectors();


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv) 
{
    if (pdrv != SDCARD_DRV_NUM)
        return RES_PARERR;
    return sdcard_get_dstatus();
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv) 
{
    if (pdrv != SDCARD_DRV_NUM)
        return RES_PARERR;
    return sdcard_init();
}


static int _sdrc2dresult(int sd_rc) 
{
    switch (sd_rc) 
    {
        case SDCARD_ERROR_OK:
            return RES_OK;
        case SDCARD_ERROR_UNUSABLE:
        case SDCARD_ERROR_NO_RESPONSE:
        case SDCARD_ERROR_NO_INIT:
        case SDCARD_ERROR_NO_DEVICE:
            return RES_NOTRDY;
        case SDCARD_ERROR_PARAMETER:
        case SDCARD_ERROR_UNSUPPORTED:
            return RES_PARERR;
        case SDCARD_ERROR_CRC:
        case SDCARD_ERROR_WRITE:
        case SDCARD_ERROR_ERASE:
        default:
            return RES_ERROR;
    }
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != SDCARD_DRV_NUM)
        return RES_PARERR;
    int rc = sdcard_read_blocks(buff, sector, count);
    return _sdrc2dresult(rc);
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != SDCARD_DRV_NUM)
        return RES_PARERR;
    int rc = sdcard_write_blocks(buff, sector, count);
    return _sdrc2dresult(rc);
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) 
{
    if (pdrv != SDCARD_DRV_NUM)
        return RES_PARERR;
    /* Buffer to send/receive control data */
    switch (cmd) 
    {
        case GET_SECTOR_COUNT:
        {
            /* Retrieves number of available sectors, the largest allowable LBA + 1, on the drive into the LBA_t variable pointed by buff.
             * This command is used by f_mkfs and f_fdisk function to determine the size of volume/partition to be created.
             * It is required when FF_USE_MKFS == 1.
             */
            static LBA_t n;
            n = sdcard_get_sectors();
            *(LBA_t *)buff = n;
            if (!n) return RES_ERROR;
            return RES_OK;
        }
        case GET_BLOCK_SIZE: 
        {
            /*
             * Retrieves erase block size of the flash memory media in unit of sector into the DWORD variable pointed by buff.
             * The allowable value is 1 to 32768 in power of 2. Return 1 if the erase block size is unknown or non flash memory media.
             * This command is used by only f_mkfs function and it attempts to align data area on the erase block boundary.
             * It is required when FF_USE_MKFS == 1.
             */
            static DWORD bs = 1;
            buff = &bs;
            return RES_OK;
        }
        case CTRL_SYNC:
            /*
             * Makes sure that the device has finished pending write process. If the disk I/O layer or storage device has a write-back cache,
             * the dirty cache data must be committed to media immediately. Nothing to do for this command if each write operation to the media
             * is completed within the disk_write function.
             */
            return RES_OK;
        default:
            return RES_PARERR;
    }
}

