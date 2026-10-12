#include "ble_meas_chr.h"
#include "ble_gatt.h"
#include "ble_gap.h"

static TimerHandle_t ble_meas_tx_timer;
static const char * TAG = "MEAS";

static void ble_meas_tx_timer_cb(TimerHandle_t xTimer)
{
      (void)xTimer;
}

void createTimer(void){
      if (ble_meas_tx_timer == NULL) {
            ble_meas_tx_timer = xTimerCreate("ble_meas_tx_timer",
                                            pdMS_TO_TICKS(1000),
                                            pdTRUE,
                                            NULL,
                                            ble_meas_tx_timer_cb);
      }
}

void ble_meas_tx_stop(void){
      if (ble_meas_tx_timer != NULL) {
            xTimerStop(ble_meas_tx_timer, 0);
      }
}

void ble_meas_tx_reset(void){
      if (ble_meas_tx_timer == NULL) {
            return;
      }

      if (xTimerReset(ble_meas_tx_timer, 0) != pdPASS) {
            ESP_LOGW(TAG, "Failed to reset BLE notification timer");
      }
}

void ble_meas_tx_send(uint16_t median_temperatture, uint16_t median_humidity){
      uint16_t device_id = 1;
      static uint8_t myFrame[6] = {0}; // 2B ID, 2B temperature, 2B humidity
      int rc;
      struct os_mbuf *om;

      uint16_t chr_val_handle = gatt_svr_chr_var_handle_getter();
      uint16_t conn_handle = gap_conn_handle_getter();

      //fill the frame with the ID
      myFrame[0] = (device_id >> 8) & 0xFF; // High byte of ID
      myFrame[1] = device_id & 0xFF;        // Low byte
      //fill the frame with the temperature
      myFrame[2] = (median_temperatture >> 8) & 0xFF; // High byte of temperature
      myFrame[3] = median_temperatture & 0xFF;        // Low byte
      //fill the frame with the humidity
      myFrame[4] = (median_humidity >> 8) & 0xFF; // High byte of humidity
      myFrame[5] = median_humidity & 0xFF;        // Low byte

      ESP_LOGI("MEAS", "conn_handle=%u, chr_val_handle=%u",
         conn_handle, chr_val_handle);

      om = ble_hs_mbuf_from_flat(myFrame, sizeof(myFrame));

      if (om == NULL) {
            ESP_LOGE("MEAS", "Failed to allocate mbuf");
            return;
      }

      rc = ble_gatts_notify_custom(conn_handle, chr_val_handle, om);

      if (rc != 0) {
            ESP_LOGE(TAG, "BLE notify failed, rc=%d (0x%x)", rc, rc);
            return;
      }

      ble_meas_tx_reset();
}

// void createTimer(void){ // czy to do usunięcia?
//       ble_meas_tx_timer = xTimerCreate("ble_meas_tx_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *) 0, ble_meas_tx_send);
// }