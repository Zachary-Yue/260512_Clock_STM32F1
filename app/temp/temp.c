/**
 * @file temp.c
 * @author Zach (zachary-yue@qq.com)
 *
 * @brief DS18B20 读取温度
 *
 * @note  - 读取温度的过程中不应当屏蔽中断
 *          - 按照编程惯例，中断里没有耗时操作，对时序影响不大
 *          - 但是关闭中断会导致其他通讯协议（如 I2C、UART）出现问题，因为 DS18B20 整个读取温度的时间在 2 ~ 3 ms 左右
 *        - GPIO 的开漏模式 + 内部上拉电阻，输出的高电平没有驱动能力，依然需要在 DQ 线上外接 4.7kΩ 上拉电阻
 *
 * @version 0.1
 * @date 2026-08-05
 *
 */
#include "temp.h"
#include "debug.h"
#include "system.h"
#include "ow.h"
#include "os_task.h"
#include "user_phr_drivers.h"

#define TAG "TEMP"

s8 t_int = 0;                  // 温度的整数部分
u8 t_deci_1 = 0;               // 温度的第一位小数部分
float t_f = 0.0f;              // 浮点数形式的温度（用来和温度参数比较）
static s16 t_raw_buf[3] = {0}; // 中值滤波
static u8 t_raw_buf_idx = 0;   // 中值滤波索引

/**
 * @brief DS18B20 任务。每 800 ms 以上执行一次（因为一次温度转换需要 750 ms 左右）。
 *
 */
void temp_task(void)
{
    task_start(TASK_TEMP);

    // uint32_t primask = __get_PRIMASK();
    // __disable_irq();
    // 将 DS18B20 的精度设为 12 位
    ow_init();
    ow_write(0xcc);
    ow_write(0x4e);
    ow_write(0);
    ow_write(0);
    ow_write(0x60);
    ow_init();
    ow_write(0xcc); // DS18B20 命令：跳过 ROM
    ow_write(0x44); // DS18B20 命令：启动一次温度转换
    // __set_PRIMASK(primask);

    task_while(1)
    {
        task_delay(800); // 延时 750 ms 以上，等待下一次温度转换完成
        {
            u8 th, tl; // 从单总线上直接读取到的数据字节
            s16 raw, raw_tmp;

            ow_init();
            ow_write(0xcc); // DS18B20 命令：跳过 ROM
            ow_write(0xbe); // DS18B20 命令：读取 scratchpad
            tl = ow_read(); // 接收 DS18B20 回传的数据字节（低字节）
            th = ow_read(); // 接收 DS18B20 回传的数据字节（高字节）
            ow_init();      // 结束通信
            ow_write(0xcc); // DS18B20 命令：跳过 ROM
            ow_write(0x44); // DS18B20 命令：启动一次温度转换

            raw = (s16)MAKEWORD(tl, th); // 将两个字节按照高低字节顺序合并为温度数据
            // 中值滤波
            t_raw_buf[t_raw_buf_idx] = raw;
            t_raw_buf_idx = (t_raw_buf_idx + 1) % 3;
            raw_tmp = mid(t_raw_buf[0], t_raw_buf[1], t_raw_buf[2]);
            t_int = raw_tmp / 16;                           // 整数部分
            t_deci_1 = ((abs(raw_tmp) % 16) * 10 + 8) / 16; // 第一位小数部分
            t_f = raw_tmp * 0.0625f;                        // 转换为摄氏度
        }
    }
    task_loop(1);

    task_end(0);
}
