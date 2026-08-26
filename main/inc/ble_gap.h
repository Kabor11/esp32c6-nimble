#pragma once

#include <common.h>

void ble_on_reset(int reason);
void ble_on_sync(void);
void ble_advertise(void);
int ble_gap_event(struct ble_gap_event *event, void *arg);
void ble_print_conn_desc(struct ble_gap_conn_desc *desc);

uint16_t gap_conn_handle_getter(void);