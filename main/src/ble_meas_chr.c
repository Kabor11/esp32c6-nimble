#include <ble_meas_chr.h>

void ble_meas_tx_stop(TimerHandle_t ble_meas_tx_timer){
      xTimerStop(ble_meas_tx_timer, 1000/portTICK_PERIOD_MS);
}

void ble_meas_tx_reset(TimerHandle_t ble_meas_tx_timer){
      int rc;
      if(xTimerReset(ble_meas_tx_timer, 1000 / portTICK_PERIOD_MS) == pdPASS){
            rc = 0;
      } else {
            rc = 1;
      }

      assert(rc == 0);
}