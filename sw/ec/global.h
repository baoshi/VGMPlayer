/* 
 * File:   global.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:13 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include <stdbool.h>


//#define _XTAL_FREQ                    1000000L

#define EC_SLAVE_ADDRESS                0x13u       // I2C slave address

#define EC_CMD_POWER_OFF                0x0A        // Turn off main power

#define EC_CMD_WATCHDOG_OFF             0x0C        // Turn off auto-off in MAIN state

#define EC_CMD_WATCHDOG_ON              0x0D        // Turn on auto-off in MAIN state when TIME_I2C_LOST_TO_OFF elapsed 

#define USB_VDD_THRESHOLD               135u        // When USB connected, VDD > USB_VDD_THRESHOLD (4.5V)

// For PRE_DFU state
#define TIME_HOLD_TO_ENTER_DFU          3000u       // Hold hotkey more than TIME_HOLD_TO_ENTER_DFU to enter DFU

// For MAIN state
#define TIME_I2C_LOST_TO_OFF            5000u       // Time from lost of uplink connection to power off

// For DFU state
#define TIME_BOOTSEL_HOLD               100u        // Time to hold BOOSEL down after power on reset

// For OFF state
#define TIME_NOTICE_OFF                 2000u


extern bool global_power_off;

#endif	/* GLOBAL_H */

