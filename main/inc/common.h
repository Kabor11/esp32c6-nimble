#pragma once

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

// const char* TAG = "NimBLE";
// const char* INFO = "INFO";
// const char* ERROR = "ERROR";
//uint8_t own_addr_type;