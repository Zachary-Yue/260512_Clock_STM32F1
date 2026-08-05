/**
 * @file ow.h
 * @author Zach (zachary-yue@qq.com)
 *
 * @brief DS18B20 单总线协议
 *
 * @version 0.1
 * @date 2026-08-03
 *
 */
#ifndef __OW_H
#define __OW_H

#include "main.h"

#define DQ_Out PAout(3)
#define DQ_In  PAin(3)

bool ow_init(void);
void ow_write(u8 dat);
u8   ow_read(void);

#endif // !__OW_H
