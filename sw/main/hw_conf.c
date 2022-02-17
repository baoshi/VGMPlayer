#include <pico.h>
#include <pico/sem.h>
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


void hw_init()
{
    // Initialize semophore for I2C exclusive access
    sem_init(&_i2c_sem, 1, 1);  // initial 1, max 1
    // Initialize I2C bus
    i2c_init(I2C_INST, I2C_BAUD_HZ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}