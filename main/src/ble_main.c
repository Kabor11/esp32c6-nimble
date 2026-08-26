#include "common.h"
#include "ble_main.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_meas_chr.h"

//static const char* INFO = "BLE_MAIN_INFO";
static const char* ERROR = "BLE_MAIN_ERROR";

void ble_init(void){
      int rc;

      esp_err_t ret = nimble_port_init();
      if(ret != ESP_OK){
            ESP_LOGE(ERROR, "Nimble port initialization failed");
            return;
      }

      ble_hs_cfg.reset_cb = ble_on_reset;
      ble_hs_cfg.sync_cb = ble_on_sync;
      //ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb; //to służy do debugowania bo tylko wypisuje rzeczy na serial monitor
      ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

      createTimer();

      rc = ble_svc_gap_device_name_set(DEVICE_NAME);
      assert(rc == 0);

      rc = gatt_svr_init();
      assert(rc == 0);
}

