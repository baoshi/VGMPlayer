#ifndef EC_H
#define EC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EC_BUTTON_MASK_NW    0x01
#define EC_BUTTON_MASK_SW    0x02
#define EC_BUTTON_MASK_PLAY  0x04
#define EC_BUTTON_MASK_NE    0x08
#define EC_BUTTON_MASK_SE    0x10

extern bool ec_charge;      // true if charging
extern float ec_battery;    // battery voltage
extern uint8_t ec_buttons;  // button states

void   ec_init(void);
int    ec_update(uint32_t now);

#ifdef __cplusplus
}
#endif

#endif