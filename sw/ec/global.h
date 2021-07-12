/* 
 * File:   global.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:13 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H


//#define _XTAL_FREQ        1000000L


#define USB_VDD_THRESHOLD   138u    // When USB connected, VDD > USB_VDD_THRESHOLD (4.6V)


// For WAKEUP state
#define TIME_IDLE_STAY_AWAKE            5000u       // Time to stay awake if no valid actions (start main procesor or enter DFU)

// For DFU state
#define TIME_NO_USB_TIMEOUT             60000u      // Time to wait in DFU mode if USB not connected
#define TIME_FIRMWARE_FLASH_TIMEOUT     60000u      // Time to wait in DFU mode if no new firmware upload

// For MAINLOOP state
#define TIME_UPLINK_LOST_TO_OFF         60000u       // Time from lost of uplink connection to power off

// For OFF state
#define TIME_NOTICE_OFF                 2000u

#endif	/* GLOBAL_H */

