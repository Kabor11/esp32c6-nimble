#pragma once

#include "common.h"
#include "esp_random.h" //to potem do wyjebania
#include "ble_gap.h"
#include "ble_gatt.h"

void ble_meas_tx_stop(void);
void ble_meas_tx_reset(void);
void ble_meas_tx_send(TimerHandle_t ev);
void createTimer(void);
