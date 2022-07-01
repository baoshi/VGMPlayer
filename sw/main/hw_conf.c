#include <pico.h>
#include <pico/sem.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>


#include "hw_conf.h"


static semaphore_t _i2c_sem; 

bool i2c_lock()
{
    return sem_acquire_timeout_ms(&_i2c_sem, I2C_TIMEOUT_US);
}


void i2c_unlock()
{
    sem_release(&_i2c_sem);
}


static void gpio_od_low(uint pin)
{
    gpio_put(pin, false);
    gpio_set_dir(pin, true);
}


static void gpio_od_high(uint pin)
{
    gpio_set_dir(pin, false);
    gpio_pull_up(pin);
    gpio_put(pin, 0);
}



// Reset bus, returns
// 0    bus reset
// -1   slave clock stretching timeout
// -2   SDA held low
static int i2c_soft_reset()
{
    // Refer: http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html
    // Removed waiting SCL low part
    // Set both SCL/SDA as input with pullup
    gpio_init(I2C_SDA_PIN);
    gpio_od_high(I2C_SDA_PIN);
    gpio_init(I2C_SCL_PIN);
    gpio_od_high(I2C_SCL_PIN);
    // Slave shall not stretch clock for more than 10ms
    sleep_ms(10);
    bool scl_low = (false == gpio_get(I2C_SCL_PIN));
    if (scl_low)
    {
        return -1;
    }
    bool sda_low = (false == gpio_get(I2C_SDA_PIN));
    int clocks = 20;    // > 2x9 clocks
    while (sda_low && (clocks > 0))     // if SDA remains low
    { 
        --clocks;
        gpio_od_low(I2C_SCL_PIN);   // SCL Low
        sleep_us(10);               // for >5us
        gpio_od_high(I2C_SCL_PIN);  // release SCL
        sleep_us(10);               // for >5us
        sda_low = (false == gpio_get(I2C_SDA_PIN));
    }
    if (sda_low) 
    {
        return -2; // I2C bus error. Could not clear. SDA data line held low
    }
    // pull SDA line low for Start or Repeated Start
    gpio_od_low(I2C_SDA_PIN);   // Make SDA low i.e. send an I2C Start or Repeated start control.
    // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
    // A Repeat Start is a Start occurring after a Start with no intervening Stop.
    sleep_us(10);
    gpio_od_high(I2C_SDA_PIN);  // Make SDA high i.e. send I2C STOP control.
    sleep_us(10);
    return 0; // all ok
}


static void i2c_bus_init()
{
    i2c_init(I2C_INST, I2C_BAUD_HZ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}


void hw_shared_resource_init()
{
    // Initialize semophore for I2C exclusive access
    sem_init(&_i2c_sem, 1, 1);  // initial 1, max 1
    // Initialize I2C bus shared by EC and WM8978
    i2c_soft_reset();
    i2c_bus_init();
}
