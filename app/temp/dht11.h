#pragma once

#include "main.h"

#define DHT11_W(x)              (PAout(2) = (x))
#define DHT11_R(x)              (PAin(2))

extern u8 hum_int;
extern bool hum_error;

void dht11_task(void);
