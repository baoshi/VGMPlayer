#ifndef EC_H
#define EC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool ec_usb;         // true if usb connected
extern bool ec_charge;      // true if charging
extern float ec_battery;    // battery voltage
extern uint8_t ec_buttons;  // button states

void ec_init(void);
bool ec_tick(void);


#define EC_BUTTON_MODE  0
#define EC_BUTTON_PLAY  1
#define EC_BUTTON_UP    2
#define EC_BUTTON_DOWN  3

int8_t ec_button_read(void);


#ifdef __cplusplus
}
#endif

#endif