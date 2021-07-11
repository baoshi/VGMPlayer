/* 
 * File:   uplink.h
 * Author: Baoshi
 *
 * Created on 2 May, 2021, 12:24 PM
 */

#ifndef UPLINK_H
#define	UPLINK_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdint.h>

  
// I2C link to main processor
// I2C slave mode, address: 0x13
//
// Master read: return 2 bytes
// 1: Status  [ X X USB CHG L R U D], Matches io_input_state
//            bit 7-6: Unimplemented
//            bit 5: USB, USB is connected = 1
//            bit 4: CHG, battery is charging = 0
//            bit 3: Right key status, debounced, pressed = 0
//            bit 2: Down key status, debounced, pressed = 0
//            bit 1: Up key status, debounced, pressed = 0
//            bit 0: Left key status, debounced, pressed = 0
// 2: Battery [7 6 5 4 3 2 1 0]    
//            V_Battery = Battery / 30.0f
// Master write: no operation
//  

void uplink_start(void);

void uplink_set_b0(uint8_t data);
void uplink_set_b1(uint8_t data);

extern volatile uint16_t uplink_activity;

void i2c_slave_bcl_isr(void);

void i2c_slave_ssp_isr(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UPLINK_H */

