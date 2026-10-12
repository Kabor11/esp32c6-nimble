#include "sht35_driver.h"
#include "i2c_pin_21_22.h"
#include "mathematic.h"
#include "ble_meas_chr.h"

static QueueHandle_t sht35_queue = NULL;

static bool sht35_crc_check(uint16_t data, uint8_t crc);




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

    sht35_soft_reset(); // Perform a soft reset on the SHT35 sensor

    esp_rom_delay_us(3000); 

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

      bool sht35_crc_temp_flag = false;
      bool sht_35_crc_hum_flag = false;

      ESP_ERROR_CHECK(i2c_master_transmit(i2c_sht35_dev_handle, start_measure, sizeof(start_measure), 100)); //100 ms timeout

      esp_rom_delay_us(20000);

      ESP_ERROR_CHECK(i2c_master_receive(i2c_sht35_dev_handle, read_buffer, sizeof(read_buffer), 1000));
      
      uint16_t rawTempTemperature = 0;
      uint16_t rawTempHumidity = 0;

      uint8_t received_CRC_Temperature = 0;
      uint8_t received_CRC_Humidity = 0;
      
      //buffer byte separation on temporary variables and CRC check
      rawTempTemperature = (read_buffer[0] << 8) | read_buffer[1];
      received_CRC_Temperature = read_buffer[2];

      rawTempHumidity = (read_buffer[3] << 8) | read_buffer[4];
      received_CRC_Humidity = read_buffer[5];

      sht35_crc_temp_flag = sht35_crc_check(rawTempTemperature, received_CRC_Temperature);
      sht_35_crc_hum_flag = sht35_crc_check(rawTempHumidity, received_CRC_Humidity);

      if(!sht35_crc_temp_flag || !sht_35_crc_hum_flag) {
            return ESP_ERR_INVALID_CRC;
      }
      
      data->rawTemperature = rawTempTemperature;
      data->rawHumidity = rawTempHumidity;

      return ESP_OK;
}

static bool sht35_crc_check(uint16_t data, uint8_t crc){
      uint8_t crc_calculated = 0xFF;
      uint8_t polynomial = 0x31; // Polynomial for CRC-8

      for (int i = 0; i < 2; i++) {
            crc_calculated ^= (data >> (8 * (1 - i))) & 0xFF;
            for (int j = 0; j < 8; j++) {
                  if (crc_calculated & 0x80) {
                        crc_calculated = (crc_calculated << 1) ^ polynomial;
                  } else {
                        crc_calculated <<= 1;
                  }
            }
      }

      return crc_calculated == crc;
}

void sht35_queue_init(void){
    if (sht35_queue == NULL) {
        sht35_queue = xQueueCreate(5, sizeof(sht35_data_t));
        if (sht35_queue == NULL) {
            ESP_LOGE("SHT35", "Failed to create SHT35 queue");
        }
    }
}

void task_sht35_read(void *pvParameters) {
    sht35_data_t sht35_data;

    while (1) {
        if (sht35_read_value(&sht35_data) == ESP_OK) {
            if (sht35_queue != NULL) {
                xQueueSend(sht35_queue, &sht35_data, portMAX_DELAY);
            }
        } else {
            ESP_LOGE("SHT35", "Failed to read SHT35 sensor data");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

void task_sht35_median_filter(void *pvParameters) {
    sht35_data_t received_data;
    uint16_t temperature_buffer[9] = {0};
    uint16_t humidity_buffer[9] = {0};
    int i = 0;

    while (1) {
            if (xQueueReceive(sht35_queue, &received_data, portMAX_DELAY) == pdTRUE) {
                
                  temperature_buffer[i] = received_data.rawTemperature;
                  humidity_buffer[i] = received_data.rawHumidity;

                  i++;
                  if (i >= 9) {
                        i = 0;

                        // Sort the buffers to find the median
                        insertion_sort(temperature_buffer, 9);
                        insertion_sort(humidity_buffer, 9);

                        uint16_t median_temperature = temperature_buffer[4]; // Median value for 9 samples
                        uint16_t median_humidity = humidity_buffer[4]; // Median value for 9 samples

                        ESP_LOGI("SHT35", "Median Temperature: 0x%04X, Median Humidity: 0x%04X", median_temperature, median_humidity);

                        ble_meas_tx_send(median_temperature, median_humidity);
                  }
                
            }
        }
}