
#pragma once

#include "user_def.h"

#define USER_FIFO_BYTENOTFOUND          (0xffff)

typedef enum {
    USER_FIFO_UNLOCK,
    USER_FIFO_LOCK
} user_fifo_lock_t;

typedef struct {
    u8 *buff;
    u16 max_size;
    u16 head;
    u16 tail;
    user_fifo_lock_t lock;
} user_fifo_t;

typedef enum {
    USER_FIFO_READ_ONLY,
    USER_FIFO_READ_AND_CLEAN
} user_fifo_read_operation_t;

void user_fifo_init(user_fifo_t *fifo, u8 *buf, u16 max_size);
void user_fifo_write_byte(user_fifo_t *fifo, u8 byt);
void user_fifo_write(user_fifo_t *fifo, const u8 *p_data, u16 len);
void user_fifo_read(user_fifo_t *fifo, u8 *p_data, u16 len, user_fifo_read_operation_t opt);
u16  user_fifo_get_used_length(const user_fifo_t *fifo);
void user_fifo_clear(user_fifo_t *fifo, u16 range);
u16  user_fifo_find_byte(user_fifo_t *fifo, u8 byte);
bool user_fifo_get_byte(user_fifo_t *fifo, u16 index, u8 *byte);
