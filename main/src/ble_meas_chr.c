#include "ble_meas_chr.h"
#include "ble_gatt.h"

static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t chr_val_handle;

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

void ble_meas_tx_send(void){
      uint8_t temperature = 0;
      static uint8_t myData[7] = {5, 0};
      int rc;
      struct os_mbuf *om;

      chr_val_handle = gatt_svr_chr_var_handle_getter();


      myData[1] = temperature;

      temperature = 6 + (esp_random() % 4);

      om = ble_hs_mbuf_from_flat(myData, sizeof(myData));
      rc = ble_gatts_notify_custom(conn_handle, chr_val_handle, om);
}