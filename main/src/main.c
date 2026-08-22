#include "common.h"
#include "ble_main.h"
#include "ble_host.h"



static const char* VERSION = "VERSION";



void app_main(void)
{
      ESP_LOGI(VERSION, "r-0.1.1");
      //int rc;

      esp_err_t ret = nvs_flash_init();
      if(ret == (ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)){
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
      }

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

