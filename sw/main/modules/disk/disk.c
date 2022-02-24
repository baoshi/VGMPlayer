#include <stddef.h>
#include <ff.h>
#include <diskio.h>
#include "event_ids.h"
#include "event_queue.h"
#include "sdcard_ll.h"
#include "disk.h"


static FATFS _sdfatfs;


static void sdcard_cd_handler(bool inserted, void* param)
{
    if (inserted)
    {
        EQ_QUICK_PUSH(EVT_DISK_INSERTED);
    }
    else
    {
        EQ_QUICK_PUSH(EVT_DISK_EJECTED);
    }
}


void disk_init()
{
    // SDCard H/W initialization
    sdcard_ll_init(sdcard_cd_handler, 0);
}


// Check if a dir is readable on the disk
// return 0 if dir is not readable and cannot mount disk (disk not present or unsupported)
// return 1 if dir is readable
// return 2 if dir is not readable but disk is valid (possiblity a new disk is inserted)
int disk_check_dir(const char* dir)
{
    int ret = 0;
    if (FR_OK == f_chdir(dir))  // Try chdir first
    {
        ret = 1;
    }
    else if (FR_OK == f_mount(&_sdfatfs, "", 1))  // f_chdir failed, we try mount the volume
    {
        ret = 2;
    }
    // can't mount
    return ret; 
}


/**
 * @brief Call this function repeatedly to receive card insert/eject notification
 * 
 * @param now       Timestamp
 * @return * int    1 if card inserted
 *                  2 if card ejected
 *                  0 if no change to card
 */
int disk_update(uint32_t now)
{
    return sdcard_ll_task(now);
}


bool disk_present()
{
    return sdcard_ll_card_detected();
}


const char* disk_result_str(FRESULT fr)
{
    switch (fr)
    {
        case FR_OK:
            return "Succeeded";
        case FR_DISK_ERR:
            return "A hard error occurred in the low level disk I/O layer";
        case FR_INT_ERR:
            return "Assertion failed";
        case FR_NOT_READY:
            return "The physical drive cannot work";
        case FR_NO_FILE:
            return "Could not find the file";
        case FR_NO_PATH:
            return "Could not find the path";
        case FR_INVALID_NAME:
            return "The path name format is invalid";
        case FR_DENIED:
            return "Access denied due to prohibited access or directory full";
        case FR_EXIST:
            return "Access denied due to prohibited access (exists)";
        case FR_INVALID_OBJECT:
            return "The file/directory object is invalid";
        case FR_WRITE_PROTECTED:
            return "The physical drive is write protected";
        case FR_INVALID_DRIVE:
            return "The logical drive number is invalid";
        case FR_NOT_ENABLED:
            return "The volume has no work area (mount)";
        case FR_NO_FILESYSTEM:
            return "There is no valid FAT volume";
        case FR_MKFS_ABORTED:
            return "The f_mkfs() aborted due to any problem";
        case FR_TIMEOUT:
            return "Could not get a grant to access the volume within defined period";
        case FR_LOCKED:
            return "The operation is rejected according to the file sharing policy";
        case FR_NOT_ENOUGH_CORE:
            return "LFN working buffer could not be allocated";
        case FR_TOO_MANY_OPEN_FILES:
            return "Number of open files > FF_FS_LOCK";
        case FR_INVALID_PARAMETER:
            return "Given parameter is invalid";
        default:
            break;
    }
    return "Unknown";
}