#include "ble_init.h"

void ble_init(void){
      int rc;

       ret = nimble_port_init();
      if(ret != ESP_OK){
            ESP_LOGE(TAG, "Nimble port initialization failed");
            return;
      }

      ble_hs_cfg.reset_cb = bleprph_on_reset;
      ble_hs_cfg.sync_cb = bleprph_on_sync;
      //ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
      ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
}