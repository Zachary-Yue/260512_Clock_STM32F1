
#include "dht11.h"
#include "os_task.h"
#include "user_tick.h"
#include "system.h"

#define DHT11_HIGH	            1
#define DHT11_LOW		        0

#define DHT11_TIMEOUT_ms        3
#define DHT11_ERROR_CNT_MAX     10
#define dht11_delay_us(x)       sys_delay_us(x)

u8 hum_int = 0;
bool hum_error = false;
static u8 hum_error_cnt = 0;

/**
 * @brief DTH读取一个字节
 * 
 * @return u8 读到的字节(u8)
 */
static u8 Dht11_ReadByte(void)
{
	u8 i = 0, Data = 0x00;
    u32 tickstart;

	for(; i < 8; i++)
    {
		Data <<= 1;
        tickstart = user_get_tick();
		while(DHT11_R() == DHT11_LOW) {
            if (user_get_tick() - tickstart >= DHT11_TIMEOUT_ms) {
                return 0;
            }
        }

		dht11_delay_us(30);

		if(DHT11_R() == DHT11_HIGH) {
			Data |= (u8)0x01;
            tickstart = user_get_tick();
			while(DHT11_R() == DHT11_HIGH) {
                if (user_get_tick() - tickstart >= DHT11_TIMEOUT_ms) {
                    return 0;
                }
            }
		}
	}
	return Data;
}


/**
 * @brief DHT11 任务
 * @details DHT11 向指定 DHT11 数据结构体读取数据包并判断数据有效性
 * 
 */
void dht11_task(void)
{
    task_start(TASK_DHT11);

    task_while(1)
    {
        // 拉低总线 18+ms，DHT11 进入工作状态
        DHT11_W(0);
        task_delay(18);

        // 不知道为什么，中断那点延时几乎不影响时序。和 51 的情况不同。这里可以不需要进入临界。
        // SYS_ENTER_CRITICAL();

        // 拉高总线 40us，DHT11 响应信号
        DHT11_W(1);
        dht11_delay_us(40);

        // 等待 DHT11 响应信号
        if(DHT11_R() == DHT11_LOW)
        {
            {
                u8 res[4];
                u8 check;
                u32 tickstart = user_get_tick();
                
                while (DHT11_R() == DHT11_LOW)
                {
                    if (user_get_tick() - tickstart >= DHT11_TIMEOUT_ms)
                    goto retry;
                }
                while(DHT11_R() == DHT11_HIGH)
                {
                    if (user_get_tick() - tickstart >= DHT11_TIMEOUT_ms)
                    goto retry;
                }

                res[0] = Dht11_ReadByte();
                res[1] = Dht11_ReadByte();
                res[2] = Dht11_ReadByte();
                res[3] = Dht11_ReadByte();
                check  = Dht11_ReadByte();

                // SYS_EXIT_CRITICAL();

                if (check == (u8)(res[0] + res[1] + res[2] + res[3])) {
                    hum_int = res[0];
                    hum_error_cnt = 0;
                    hum_error = false;
                }
                else {
                    if (hum_error_cnt < DHT11_ERROR_CNT_MAX) {
                        hum_error_cnt++;
                    }
                    else {
                        hum_error = true;
                    }
                }
            }

            task_delay(1000);
        }
        else {
retry:
            // SYS_EXIT_CRITICAL();
            task_delay(200); // 如果 DHT11 没有响应，延时 200ms 后重试
            if (hum_error_cnt < DHT11_ERROR_CNT_MAX) {
                hum_error_cnt++;
            }
            else {
                hum_error = true;
            }
        }
    }
    task_loop(1);
    task_end(0);
}
