#include "sht35_driver.h"
#include "i2c_pin_21_22.h"

#define SHT35_I2C_ADDRESS 0x45

static i2c_master_dev_handle_t i2c_sht35_dev_handle;
