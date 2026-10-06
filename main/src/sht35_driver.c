#include "sht35_driver.h"
#include "i2c_pin_21_22.h"



#define SHT35_I2C_ADDRESS 0x45

#define REPEATABILITY_LOW_CLK_STRETCHING_ON {0x2C, 0x10}
#define REPEATABILITY_MEDIUM_CLK_STRETCHING_ON {0x2C, 0x1D}
#define REPEATABILITY_HIGH_CLK_STRETCHING_ON {0x2C, 0x06}

#define REPEATABILITY_LOW_CLK_STRETCHING_OFF {0x24, 0x16}
#define REPEATABILITY_MEDIUM_CLK_STRETCHING_OFF {0x24, 0x0B}
#define REPEATABILITY_HIGH_CLK_STRETCHING_OFF {0x24, 0x00}

static i2c_master_dev_handle_t i2c_sht35_dev_handle;

esp_err_t sht35_init(void) {
    // Initialize the I2C master bus
    ESP_ERROR_CHECK(i2c_master_init());

    // Create a new I2C device handle for the SHT35 sensor
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT35_I2C_ADDRESS,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_config, &i2c_sht35_dev_handle));

    return ESP_OK;
}

esp_err_t sht35_soft_reset(void){
      uint8_t reset_cmd[2] = {0x30, 0xA2}; // Soft reset command for SHT35
      ESP_ERROR_CHECK(i2c_master_transmit(i2c_sht35_dev_handle, reset_cmd, sizeof(reset_cmd), 100)); //100 ms timeout
      return ESP_OK;
}

esp_err_t sht35_read_value(sht35_data_t *data){
      uint8_t start_measure[2] = REPEATABILITY_HIGH_CLK_STRETCHING_OFF; // Use high repeatability with clock stretching
      uint8_t read_buffer[6]; // 2 bytes for temperature, 1 byte CRC, 2 bytes for humidity, 1 byte CRC

      ESP_ERROR_CHECK(i2c_master_transmit(i2c_sht35_dev_handle, start_measure, sizeof(start_measure), 100)); //100 ms timeout
      
      uint16_t rawTempTemperature = 0;
      uint16_t rawTempHumidity = 0;

      uint8_t received_CRC_Temperature = 0;
      uint8_t received_CRC_Humidity = 0;
      
      //buffer byte separation on temporary variables and CRC check
      rawTempTemperature = (read_buffer[0] << 8) | read_buffer[1];
      received_CRC_Temperature = read_buffer[2];

      rawTempHumidity = (read_buffer[3] << 8) | read_buffer[4];
      received_CRC_Humidity = read_buffer[5];

      // CRC check can be implemented here if needed

      return ESP_OK;
}