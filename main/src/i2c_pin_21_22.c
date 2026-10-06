#include "i2c_pin_21_22.h"

// SDA 21
// SCL 22

#define I2C_MASTER_SCL_IO           22          
#define I2C_MASTER_SDA_IO           21          
#define I2C_MASTER_NUM              I2C_NUM_0

esp_err_t i2c_master_init(void){
      i2c_master_bus_config_t bus_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = I2C_MASTER_NUM,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .scl_io_num = I2C_MASTER_SCL_IO,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = 1,
      };
      ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus_handle));
      return ESP_OK;
}
