#include "ble_host.h"

void ble_host_task(void *param){
      //ESP_LOGI(TAG, "NimBLE host task started");
      nimble_port_run();
      nimble_port_freertos_deinit();
}