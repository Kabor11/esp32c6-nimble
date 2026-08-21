#include "common.h"

#define DEVICE_NAME "KUTNAPLETES"

void bleprph_on_reset(int reason);
void bleprph_on_sync(void);
void bleprph_advertise(void);
int bleprph_gap_event(struct ble_gap_event *event, void *arg);
void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc);
void bleprph_host_task(void *param);

const char* TAG = "NimBLE";
const char* INFO = "INFO";
const char* ERROR = "ERROR";
uint8_t own_addr_type;

void app_main(void)
{
      int rc;

      esp_err_t ret = nvs_flash_init();
      if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND){
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
      }

      ret = nimble_port_init();
      if(ret != ESP_OK){
            ESP_LOGE(TAG, "Nimble port initialization failed");
            return;
      }

      ble_hs_cfg.reset_cb = bleprph_on_reset;
      ble_hs_cfg.sync_cb = bleprph_on_sync;
      //ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
      ble_hs_cfg.store_status_cb = ble_store_util_status_rr; 

      //zaraz za tym w tutorialu od esspressif są opcje przekazywania kluczy i szyfrowania

      //rc = gatt_svr_init();
      //assert(rc == 0);

      rc = ble_svc_gap_device_name_set(DEVICE_NAME);
      assert(rc == 0);

      //ble_store_config_init();

      nimble_port_freertos_init(bleprph_host_task);

      //rc = scli_init();
      //if(rc != ESP_OK){
      //      ESP_LOGE(TAG, "scli init failed");
      //}
}

void bleprph_on_reset(int reason){
     ESP_LOGI(ERROR, "Resetting state reason = %d", reason);
}

void bleprph_on_sync(void){
      int rc;
      rc = ble_hs_util_ensure_addr(0);
      assert(rc==0);

      rc = ble_hs_id_infer_auto(0, &own_addr_type);
      if(rc != 0){
            return;
      }

      bleprph_advertise();
}

void bleprph_advertise(void){
      struct ble_gap_adv_params adv_params;
      struct ble_hs_adv_fields adv_fields;
      int rc;
      memset(&adv_fields, 0, sizeof adv_fields);

      adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

      const char* name = ble_svc_gap_device_name();
      adv_fields.name = (uint8_t *)name;
      adv_fields.name_len = strlen(name);
      adv_fields.name_is_complete = 1;

      rc = ble_gap_adv_set_fields(&adv_fields);
      if(rc != 0){
            return;
      }

      memset(&adv_params, 0, sizeof adv_params);
      adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
      adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

      rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, bleprph_gap_event, NULL);

      if(rc != 0){
            return;
      }
}

int bleprph_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:

        ESP_LOGI(INFO, "connection %s; status=%d ",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status == 0) {

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);

           // bleprph_print_conn_desc(&desc);

            ESP_LOGI(INFO, "\n");
        } else {

            ESP_LOGI(INFO, "\n");
            ESP_LOGI(INFO, "Connection failed; resuming advertising\n");

            bleprph_advertise();
        }

        return 0;


    case BLE_GAP_EVENT_DISCONNECT:

        ESP_LOGI(INFO, "disconnect; reason=%d ",
                    event->disconnect.reason);

        //bleprph_print_conn_desc(&event->disconnect.conn);

        ESP_LOGI(INFO, "\n");

        /* Po rozłączeniu ponownie uruchamiamy advertising. */
        bleprph_advertise();

        return 0;


    case BLE_GAP_EVENT_SUBSCRIBE:

        ESP_LOGI(INFO,
                    "subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                    event->subscribe.conn_handle,
                    event->subscribe.attr_handle,
                    event->subscribe.reason,
                    event->subscribe.prev_notify,
                    event->subscribe.cur_notify,
                    event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);

        return 0;


    case BLE_GAP_EVENT_MTU:

        ESP_LOGI(INFO,
                    "mtu update event; conn_handle=%d cid=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.channel_id,
                    event->mtu.value);

        return 0;
    }

    return 0;
}

// void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
// {
//     ESP_LOGI(INFO, "handle=%d our_ota_addr_type=%d our_ota_addr=",
//                 desc->conn_handle, desc->our_ota_addr.type);
//     print_addr(desc->our_ota_addr.val);
//     ESP_LOGI(INFO, " our_id_addr_type=%d our_id_addr=",
//                 desc->our_id_addr.type);
//     print_addr(desc->our_id_addr.val);
//     ESP_LOGI(INFO, " peer_ota_addr_type=%d peer_ota_addr=",
//                 desc->peer_ota_addr.type);
//     print_addr(desc->peer_ota_addr.val);
//     ESP_LOGI(INFO, " peer_id_addr_type=%d peer_id_addr=",
//                 desc->peer_id_addr.type);
//     print_addr(desc->peer_id_addr.val);
//     ESP_LOGI(INFO, " conn_itvl=%d conn_latency=%d supervision_timeout=%d "
//                 "encrypted=%d authenticated=%d bonded=%d\n",
//                 desc->conn_itvl, desc->conn_latency,
//                 desc->supervision_timeout,
//                 desc->sec_state.encrypted,
//                 desc->sec_state.authenticated,
//                 desc->sec_state.bonded);
// }

void bleprph_host_task(void *param){
      ESP_LOGI(TAG, "NimBLE host task started");
      nimble_port_run();
      nimble_port_freertos_deinit();
}
