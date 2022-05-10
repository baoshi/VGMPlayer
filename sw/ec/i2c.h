/* 
 * File:   i2c.h
 * Author: Baoshi
 *
 * Created on 9 May, 2022, 12:24 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdint.h>

  
// I2C link to main processor
// I2C slave mode, address: 0x13
//
// Master read: return 2 bytes
// 1: Status  [ x x CHG  SE NE PLAY SW NW]
//            bit 7: Unimplemented
//            bit 6: Unimplemented
//            bit 5: CHG battery is charging = 1
//            bit 4: SE key status, debounced, pressed = 1
//            bit 3: NE key status, debounced, pressed = 1
//            bit 2: PLAY key status, debounced, pressed = 1
//            bit 1: SW key status, debounced, pressed = 1
//            bit 0: NW key status, debounced, pressed = 1
// 2: Battery [7 6 5 4 3 2 1 0]    
//            V_Battery = Battery / 30.0f
// Master write: no operation
//  

extern volatile uint8_t i2c_recent_activity;
    
void i2c_start(void);
void i2c_stop(void);
void i2c_track_activity(void);

void i2c_slave_bcl_isr(void);

void i2c_slave_ssp_isr(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

