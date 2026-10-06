#pragma once
#include "common.h"

typedef struct {
      uint16_t rawTemperature;
      uint16_t rawHumidity;
}     sht35_data_t;

esp_err_t sht35_init(void);
esp_err_t sht35_soft_reset(void);
esp_err_t sht35_read_value(sht35_data_t *data);