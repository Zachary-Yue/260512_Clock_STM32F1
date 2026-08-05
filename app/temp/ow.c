/**
 * @file ow.c
 * @author Zach (zachary-yue@qq.com)
 *
 * @brief DS18B20 单总线协议
 * 
 * @note  - GPIO 的开漏模式 + 内部上拉电阻，输出的高电平没有驱动能力，依然需要在 DQ 线上外接 4.7kΩ 上拉电阻
 *
 * @version 0.1
 * @date 2026-08-03
 *
 */
#include "ow.h"
#include "system.h"

#define DQ_Set(x) 		(DQ_Out = (u32)(x)) // 需要在 DQ 线上外接 4.7kΩ 上拉电阻
#define DQ_Read() 		(DQ_In)
#define ow_delay(x) 	sys_delay_us(x) // 这里使用了 sys_delay_us 函数来实现延时，单位是微秒。

bool ow_init(void)
{
	bool initflag = 0;

	DQ_Set(1);
	ow_delay(2);
	DQ_Set(0);
	ow_delay(480);
	DQ_Set(1);
	ow_delay(80);
	initflag = DQ_Read();
	ow_delay(400);

	return initflag;
}

void ow_write(u8 dat)
{
	u8 i;
	for (i = 0; i < 8; i++)
	{
		DQ_Set(0);
		ow_delay(5);
		DQ_Set(dat & 0x01);
		ow_delay(55);
		DQ_Set(1);
		dat >>= 1;
		ow_delay(1);
	}
}

u8 ow_read(void)
{
	u8 i;
	u8 dat;

	for (i = 0; i < 8; i++)
	{
		DQ_Set(0);
		ow_delay(2);
		dat >>= 1;
		DQ_Set(1);
		ow_delay(8);
		if (DQ_Read())
		{
			dat |= 0x80;
		}
		ow_delay(51);
	}
	return dat;
}
