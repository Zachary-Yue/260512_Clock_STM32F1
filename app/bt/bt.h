
#pragma once

#include "dx_bt37.h"

extern dx_bt37_t bt37;

#define bt_send() dx_bt37_send(&bt37)
#define bt_recv() dx_bt37_recv(&bt37)
#define bt_rx_cplt_handler(Size) dx_bt37_rx_cplt_handler(&bt37, Size)

#define bt_printf(fmt, ...) dx_bt37_printf(&bt37, fmt, ##__VA_ARGS__)
#define bt_send_all() dx_bt37_send_all(&bt37)
#define bt_disconnect() dx_bt37_disconnect(&bt37)

void bt_init(void);
void bt_change_baud(u32 baudrate);
