
#include "user_fifo.h"

static void user_fifo_add_head_index(user_fifo_t *fifo, u16 len);
static void user_fifo_add_tail_index(user_fifo_t *fifo, u16 len);


/**
 * @brief fifo 结构体初始化。
 * @note  fifo 实际能使用的长度是其管理的内存数组长度 -1。
 * 
 * @param fifo      fifo 结构体
 * @param buf       结构体管理的内存数组
 * @param max_size   上述数组的长度
 */
void user_fifo_init(user_fifo_t *fifo, u8 *buf, u16 max_size)
{
    if (fifo == NULL || buf == NULL || max_size <= 1) return;

    fifo->buff = buf;
    fifo->head = 0;
    fifo->tail = 0;
    fifo->max_size = max_size;
    fifo->lock = USER_FIFO_UNLOCK;
}

/**
 * @brief 向 fifo 中写入一字节数据
 * 
 * @param fifo fifo 结构体
 * @param byt  一字节数据
 */
void user_fifo_write_byte(user_fifo_t *fifo, u8 byt)
{
    if (fifo == NULL) return;
    if (fifo->lock == USER_FIFO_LOCK) return;

    fifo->lock = USER_FIFO_LOCK;

    if (user_fifo_get_used_length(fifo) + 1 < fifo->max_size)
    {
        fifo->buff[fifo->tail] = byt;
        user_fifo_add_tail_index(fifo, 1);
    }

    fifo->lock = USER_FIFO_UNLOCK;
}

/**
 * @brief 向 fifo 中写入一定长度数据，len 的最大值为 fifo 可用空间的大小
 *        （max_size - 1 - 已使用长度）。如果 len 超过这个值，函数会自动限制 len 的大小。
 * 
 * @param fifo  fifo 结构体
 * @param p_data  数据源地址
 * @param len  要写入的长度
 */
void user_fifo_write(user_fifo_t *fifo, const u8 *p_data, u16 len)
{
    if (fifo == NULL || p_data == NULL || len == 0) return;
    if (fifo->lock == USER_FIFO_LOCK) return;

    fifo->lock = USER_FIFO_LOCK;

    u16 max_write_len = fifo->max_size - 1 - user_fifo_get_used_length(fifo);
    if (len > max_write_len) len = max_write_len;   // 限制写入长度，确保不会覆盖未读数据

    u16 len_to_end = fifo->max_size - fifo->tail;  // 计算尾指针后面有多少空间
    if (len <= len_to_end)                    // 长度小于等于尾指针后面的空间
    {
        memcpy(fifo->buff + fifo->tail, p_data, len);
        user_fifo_add_tail_index(fifo, len);
    }
    else                                    // 长度大于尾指针后面的空间
    {
        memcpy(fifo->buff + fifo->tail, p_data, len_to_end);
        memcpy(fifo->buff, p_data + len_to_end, len - len_to_end);
        user_fifo_add_tail_index(fifo, len);
    }

    fifo->lock = USER_FIFO_UNLOCK;
}

/**
 * @brief 从 fifo 中读取一定长度数据。
 * 
 * @param fifo  fifo 结构体
 * @param p_data 数据目的地
 * @param len   要读取的长度
 * @param opt   读取选项
 */
void user_fifo_read(user_fifo_t *fifo, u8 *p_data, u16 len, user_fifo_read_operation_t opt)
{
    u16 len_to_end;

    if (fifo == NULL || p_data == NULL || len == 0) return;  // 参数合法
    if (fifo->lock == USER_FIFO_LOCK) return;                // 没锁
    if (len > user_fifo_get_used_length(fifo)) return;       // 长度合法

    fifo->lock = USER_FIFO_LOCK;     // 锁上

    len_to_end = fifo->max_size - fifo->head;  // 计算尾指针后面有多少空间
    if (len <= len_to_end)                    // 长度小于等于尾指针后面的空间
    {
        memcpy(p_data, fifo->buff + fifo->head, len);
    }
    else                                    // 长度大于尾指针后面的空间
    {
        memcpy(p_data, fifo->buff + fifo->head, len_to_end);
        memcpy(p_data + len_to_end, fifo->buff, len - len_to_end);
    }

    if (opt == USER_FIFO_READ_AND_CLEAN)
    {
        user_fifo_add_head_index(fifo, len);       // 移动头指针
    }

    fifo->lock = USER_FIFO_UNLOCK;               // 解锁
}

/**
 * @brief 计算已使用多少空间。
 * 
 * @param fifo fifo 结构体
 * @return u16 使用了多少空间（字节）
 */
u16 user_fifo_get_used_length(const user_fifo_t *fifo)
{
    if (fifo == NULL || fifo->max_size == 0) return 0;
    return (fifo->tail + fifo->max_size - fifo->head) % fifo->max_size;
}

/**
 * @brief 清空 fifo 索引小于 range 的数据。
 * @note  当 range > 已使用长度时，fifo 会被全部清空，
 *        这种情况和 range 给<user_fifo_get_used_length(fifo)>是一样的。
 * 
 * @param fifo  fifo 结构体。
 * @param range 索引
 */
void user_fifo_clear(user_fifo_t *fifo, u16 range)
{
    if (fifo == NULL) return;
    if (fifo->lock == USER_FIFO_LOCK) return;

    fifo->lock = USER_FIFO_LOCK;
    range > user_fifo_get_used_length(fifo) ? (range = user_fifo_get_used_length(fifo)) : (void)0;
    user_fifo_add_head_index(fifo, range);
    fifo->lock = USER_FIFO_UNLOCK;
}


/**
 * @brief 在 fifo 已使用的区间上找某个字节第一次出现的位置，返回位置索引（0 ~ max_size - 2）。
 * 
 * @param fifo fifo 实例
 * @param byte 想找的字节数据
 * @return u16 该字节第一次出现位置的索引。注意 0 也是索引，相当于数组的 0 号元素。
 *         而返回 65535 表示没有找到，其宏定义是 USER_FIFO_BYTENOTFOUND。
 */
u16 user_fifo_find_byte(user_fifo_t *fifo, u8 byte)
{
    u16 index = USER_FIFO_BYTENOTFOUND;
    if (fifo == NULL) return USER_FIFO_BYTENOTFOUND;
    if (fifo->max_size == 0) return USER_FIFO_BYTENOTFOUND;
    if (fifo->lock == USER_FIFO_LOCK) return USER_FIFO_BYTENOTFOUND;

    fifo->lock = USER_FIFO_LOCK;

    if (fifo->tail > fifo->head)
    {
        u8 *p_res = memchr(fifo->buff + fifo->head, byte, fifo->tail - fifo->head);
        if (p_res != NULL) index = p_res - (fifo->buff + fifo->head);
    }
    else if (fifo->tail < fifo->head)
    {
        u8 *p_res = memchr(fifo->buff + fifo->head, byte, fifo->max_size - fifo->head);
        if (p_res == NULL)
        {
            p_res = memchr(fifo->buff, byte, fifo->tail);
            if (p_res != NULL) index = (p_res - fifo->buff) + (fifo->max_size - fifo->head);
        }
        else index = p_res - (fifo->buff + fifo->head);
    }
    // else if (fifo->tail == fifo->head);      // 这种情况 fifo 使用长度为 0

    fifo->lock = USER_FIFO_UNLOCK;
    return index;
}


/**
 * @brief 根据索引获得 fifo 数组元素的值
 * @note  fifo 随时可以看作是一个长度为 max_size - 1 的数组，其 0 号元素就是最早进入队列的元素。
 * 
 * @param fifo  fifo 实例
 * @param index fifo 数组索引（0 ~ max_size - 2）
 * @param byte  数组元素（地址）
 * @return bool 操作是否成功
 */
bool user_fifo_get_byte(user_fifo_t *fifo, u16 index, u8 *byte)
{
    if (fifo == NULL || byte == NULL) return false;
    if (index >= user_fifo_get_used_length(fifo)) return false;
    fifo->lock = USER_FIFO_LOCK;

    if (index >= fifo->max_size - fifo->head) *byte = fifo->buff[index - (fifo->max_size - fifo->head)];
    else *byte = fifo->buff[fifo->head + index];

    fifo->lock = USER_FIFO_UNLOCK;
    return true;
}


static void user_fifo_add_head_index(user_fifo_t *fifo, u16 len)
{
    fifo->head += len;
    fifo->head %= fifo->max_size;
}

static void user_fifo_add_tail_index(user_fifo_t *fifo, u16 len)
{
    fifo->tail += len;
    fifo->tail %= fifo->max_size;
}
