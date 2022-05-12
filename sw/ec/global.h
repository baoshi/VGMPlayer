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

// For PRE_DFU state
#define TIME_HOLD_TO_ENTER_DFU          3000u       // Hold hotkey more than TIME_HOLD_TO_ENTER_DFU to enter DFU

// For MAIN state
#define TIME_I2C_LOST_TO_OFF            5000u       // Time from lost of uplink connection to power off

// For DFU state
#define TIME_BOOTSEL_HOLD               100u        // Yime to hold BOOSEL down after power on reset

// For OFF state
#define TIME_NOTICE_OFF                 2000u

#endif	/* GLOBAL_H */

