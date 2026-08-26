#include "ble_gatt.h"

static const ble_uuid128_t gatt_svr_svc_uuid = 
      BLE_UUID128_INIT(0x7b, 0x4e, 0x2c, 0x91, 0x6a, 0x53, 0x4f, 0x18,
                     0x9d, 0x72, 0x31, 0xc8, 0xa6, 0xe5, 0x04, 0xbf);

static uint8_t gatt_svr_chr_val[7]; //z tej zmiennej potem zrobić albo struktuę, albo tablicę żeby wysłać ramkę
static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0xc1, 0x5f, 0x83, 0xa2, 0x9e, 0x47, 0x4b, 0x61,
                     0xa0, 0xd3, 0x72, 0xe9, 0x4c, 0x81, 0x65, 0xfa);

static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svc_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &gatt_svr_chr_val_handle,
            },
            {
                0,
            },
        },
    },
    {
        0,
    }
};

static int gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    int rc;

    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        if(attr_handle == gatt_svr_chr_val_handle){
            rc = os_mbuf_append(ctxt->om, &gatt_svr_chr_val, sizeof(gatt_svr_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        break;
    
    default:
        break;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

int gatt_svr_init(void){
    int rc;

    #if CONFIG_BT_NIMBLE_GAP_SERVICE
        ble_svc_gap_init();
    #endif

    #if MYNEWT_VAL(BLE_GATTS)
        ble_svc_gatt_init();
    #endif

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    ESP_LOGI("GATT", "count_cfg rc = %d", rc);
    if(rc != 0){
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    ESP_LOGI("GATT", "add_svcs rc = %d", rc);
    if(rc != 0){
        return rc;
    }
    
    return 0;
}

uint16_t gatt_svr_chr_var_handle_getter(void){
    return gatt_svr_chr_val_handle;
}