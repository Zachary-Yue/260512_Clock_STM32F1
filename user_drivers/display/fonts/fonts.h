
#pragma once

#define OLED_ROW(r) (((r) - 1) * 2) // range: [1, 4]
#define OLED_COL(c) (((c) - 1) * 8) // range: [1, 16]

#include "ascii_1608.h"
#include "cn16.h"
#include "icon16.h"
#include "num3216.h"
#include "cn1608.h"
