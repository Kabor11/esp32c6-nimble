#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_meas_chr.h"

static const char* INFO = "BLE_GAP_INFO";
static const char* ERROR = "BLE_GAP_ERROR";
static uint8_t own_addr_type;
static uint16_t conn_handle = BLE_HS_CONN_HANDLE_NONE;;

void ble_on_reset(int reason){
     ESP_LOGI(ERROR, "Resetting state reason = %d", reason);
}

void ble_on_sync(void){
      int rc;
      rc = ble_hs_util_ensure_addr(0);
      assert(rc==0);

      rc = ble_hs_id_infer_auto(0, &own_addr_type);
      if(rc != 0){
            return;
      }

      ble_advertise();
}

void ble_advertise(void){
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

      rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);

      if(rc != 0){
            return;
      }
}

int ble_gap_event(struct ble_gap_event *event, void *arg)
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
            conn_handle = event->connect.conn_handle;

            ESP_LOGI(INFO, "Connected, conn_handle=%u", conn_handle);

           // ble_print_conn_desc(&desc);

            ESP_LOGI(INFO, "\n");
        } else {

            ESP_LOGI(INFO, "\n");
            ESP_LOGI(INFO, "Connection failed; resuming advertising\n");

            ble_advertise();
        }

        return 0;


    case BLE_GAP_EVENT_DISCONNECT:

        ESP_LOGI(INFO, "disconnect; reason=%d ",
                    event->disconnect.reason);

        //ble_print_conn_desc(&event->disconnect.conn);
        conn_handle = BLE_HS_CONN_HANDLE_NONE;

        ESP_LOGI(INFO, "Disconnected, old conn_handle=%u",
         event->disconnect.conn.conn_handle);

        ESP_LOGI(INFO, "\n");

        /* Po rozłączeniu ponownie uruchamiamy advertising. */
        ble_advertise();

        return 0;


case BLE_GAP_EVENT_SUBSCRIBE:

    ESP_LOGI(INFO,
             "subscribe event; conn_handle=%d attr_handle=%d "
             "reason=%d prevn=%d curn=%d previ=%d curi=%d",
             event->subscribe.conn_handle,
             event->subscribe.attr_handle,
             event->subscribe.reason,
             event->subscribe.prev_notify,
             event->subscribe.cur_notify,
             event->subscribe.prev_indicate,
             event->subscribe.cur_indicate);

    if (event->subscribe.attr_handle == gatt_svr_chr_var_handle_getter()) {

        if (event->subscribe.cur_notify) {
            ble_meas_tx_reset();
        } else {
            ble_meas_tx_stop();
        }

    }

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

uint16_t gap_conn_handle_getter(void){
    return conn_handle;
}

// void ble_print_conn_desc(struct ble_gap_conn_desc *desc)
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