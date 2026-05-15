
#pragma once

#include "main.h"

typedef struct i2c_oled_t {
    u8 *display_buf; // display buffer of 0.96 inch oled (usually 128 * 8)
    bool (*i2c_mem_write_block)(u8 slvaddr, u8 memaddr, u8 *data, u16 len); // i2c operation function
    bool (*i2c_mem_write_dma)(u8 slvaddr, u8 memaddr, u8 *data, u16 len); // i2c operation function
    u16 display_buf_size; // display buffer size (usually 128 * 8)
    u16 width; // horizon-size of the screen (usually 128)
    u16 height; // vertical size of the screen (usually 64)
    
    u16 x_size_in_col; // screen x size in columns, usually equal to width (128 cols)
    u16 y_size_in_row; // screen y size in rows, usually equal to height / 8 (8 rows)

    u8 i2c_addr; // i2c address of the screen
    bool is_init;
    bool scr_is_on;
} i2c_oled_t;

typedef struct i2c_oled_cfg_t {
    u8 *display_buf; // display buffer of 0.96 inch oled (usually 128 * 8)
    bool (*i2c_mem_write_block)(u8 slvaddr, u8 memaddr, u8 *data, u16 len); // i2c operation function
    bool (*i2c_mem_write_dma)(u8 slvaddr, u8 memaddr, u8 *data, u16 len); // i2c operation function
    u16 display_buf_size; // display buffer size (usually 128 * 8)
    u16 width; // horizon-size of the screen (usually 128)
    u16 height; // vertical size of the screen (usually 64)
    u8 i2c_addr; // i2c address of the screen
} i2c_oled_cfg_t;

typedef struct i2c_oled_font_t {
    const u8 *data; // font data in a linear array
    u32 size;       // number of characters / icons
    u8 width;       // width (horizon-size) of each character
    u8 height;      // height (vertical-size) of each character
} i2c_oled_font_t;

bool i2c_oled_init(i2c_oled_t *oled, i2c_oled_cfg_t *cfg);
void i2c_oled_clear(i2c_oled_t *oled);
bool i2c_oled_refresh(i2c_oled_t *oled);
bool i2c_oled_scr_on_off(i2c_oled_t *oled, bool on);
void i2c_oled_show_char(i2c_oled_t *oled, u8 row, u8 col, const i2c_oled_font_t *font, u32 num);
