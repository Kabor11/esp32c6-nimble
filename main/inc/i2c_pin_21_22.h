#pragma once
#include "common.h"
#include "driver/i2c_master.h"

#define I2C_MASTER_SCL_IO           22          
#define I2C_MASTER_SDA_IO           21          
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_FREQ_HZ (100*1000)      //100 kHz

extern i2c_master_bus_handle_t i2c_bus_handle;

esp_err_t i2c_master_init(void);


