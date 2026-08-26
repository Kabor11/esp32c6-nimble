#include "ble_meas_chr.h"
#include "ble_gatt.h"
#include "ble_gap.h"

static TimerHandle_t ble_meas_tx_timer;
static const char * TAG = "MEAS";


void ble_meas_tx_stop(void){
      xTimerStop(ble_meas_tx_timer, 1000/portTICK_PERIOD_MS);
}

void ble_meas_tx_reset(void){
      int rc;
      if(xTimerReset(ble_meas_tx_timer, 1000 / portTICK_PERIOD_MS) == pdPASS){
            rc = 0;
      } else {
            rc = 1;
      }

      assert(rc == 0);
}

void ble_meas_tx_send(TimerHandle_t ev){
      uint8_t temperature = 0;
      static uint8_t myData[7] = {5, 0};
      int rc;
      struct os_mbuf *om;

      uint16_t chr_val_handle = gatt_svr_chr_var_handle_getter();
      uint16_t conn_handle = gap_conn_handle_getter();

      temperature = 6 + (esp_random() % 4);

      myData[1] = temperature;

      

      om = ble_hs_mbuf_from_flat(myData, sizeof(myData));
      rc = ble_gatts_notify_custom(conn_handle, chr_val_handle, om);

      assert(rc == 0);
      ESP_LOGI(TAG, "temperature: %u", temperature);

      ble_meas_tx_reset();
}

void createTimer(void){
      ble_meas_tx_timer = xTimerCreate("ble_meas_tx_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *) 0, ble_meas_tx_send);
}