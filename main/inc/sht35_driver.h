#pragma once
#include "common.h"

esp_err_t sht35_init(void);
esp_err_t sht35_soft_reset(void);
esp_err_t sht35_read_value(float *temperature, float *humidity);