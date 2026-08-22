#include "common.h"
#include "ble_main.h"

void ble_host_task(void *param);



void app_main(void)
{
      //int rc;

      esp_err_t ret = nvs_flash_init();
      if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND){
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
      }

      // ret = nimble_port_init();
      // if(ret != ESP_OK){
      //       ESP_LOGE(TAG, "Nimble port initialization failed");
      //       return;
      // }

      // ble_hs_cfg.reset_cb = ble_on_reset;
      // ble_hs_cfg.sync_cb = ble_on_sync;
      // ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
      // ble_hs_cfg.store_status_cb = ble_store_util_status_rr; 

      ble_init();

      //zaraz za tym w tutorialu od esspressif są opcje przekazywania kluczy i szyfrowania

      //rc = gatt_svr_init();
      //assert(rc == 0);

      //ble_store_config_init();

      nimble_port_freertos_init(ble_host_task);

      //rc = scli_init();
      //if(rc != ESP_OK){
      //      ESP_LOGE(TAG, "scli init failed");
      //}
}



void ble_host_task(void *param){
      //ESP_LOGI(TAG, "NimBLE host task started");
      nimble_port_run();
      nimble_port_freertos_deinit();
}
