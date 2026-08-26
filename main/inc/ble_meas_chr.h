#pragma once

#include "common.h"
#include "esp_random.h" //to potem do wyjebania
#include "ble_gap.h"
#include "ble_gatt.h"

void ble_meas_tx_stop(TimerHandle_t ble_meas_tx_timer);
void ble_meas_tx_reset(TimerHandle_t ble_meas_tx_timer);
void ble_meas_tx_send(void);
