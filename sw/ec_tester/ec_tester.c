#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17


int main()
{
    uint8_t data[2] = { 0, 0 };
    uint8_t shadow[2] = { 0, 0 };

    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    while (1) 
    {
        if (i2c_read_timeout_us(I2C_PORT, 0x13, data, 2, false, 1000) == 2)
        {
            // Master read: return 2 bytes
            // 1: Status  [ x x CHG  SE NE PLAY SW NW]
            //            bit 7: Unimplemented
            //            bit 6: Unimplemented
            //            bit 5: CHARGE battery is charging = 1
            //            bit 4: SE key status, debounced, pressed = 1
            //            bit 3: NE key status, debounced, pressed = 1
            //            bit 2: PLAY key status, debounced, pressed = 1
            //            bit 1: SW key status, debounced, pressed = 1
            //            bit 0: NW key status, debounced, pressed = 1
            // 2: Battery [7 6 5 4 3 2 1 0]    
            //            V_Battery = Battery / 30.0f
            if (shadow[0] != data[0])
            {
                printf("IO: ");
                if (data[0] & 0x01) printf("NW ");
                if (data[0] & 0x02) printf("SW ");
                if (data[0] & 0x08) printf("NE ");
                if (data[0] & 0x10) printf("SE ");
                if (data[0] & 0x04) printf("PL ");
                if (data[0] & 0x20) printf("CHG");
                printf("\n");
                shadow[0] = data[0];
            }
            if (abs(shadow[1] - data[1]) >= 3)
            {
                printf("Battery: %.1fv\n", data[1] / 30.0f);
                shadow[1] = data[1];
            }
        }
        else
        {
            printf("No data\n");
        }
        sleep_ms(100);
    }


    return 0;
}
