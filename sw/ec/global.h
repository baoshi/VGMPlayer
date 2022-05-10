/* 
 * File:   global.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:13 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H


//#define _XTAL_FREQ                    1000000L


#define USB_VDD_THRESHOLD               135u        // When USB connected, VDD > USB_VDD_THRESHOLD (4.5V)

// For MAIN state
#define TIME_I2C_LOST_TO_OFF            5000u       // Time from lost of uplink connection to power off

// For OFF state
#define TIME_NOTICE_OFF                 2000u

#endif	/* GLOBAL_H */

