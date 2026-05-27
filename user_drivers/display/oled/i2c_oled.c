
#include "i2c_oled.h"
#include "sys_err.h"

#define TAG "i2c_oled"

#define OLED_CONTROLBYTE_CMD_STREAM     (0x00) // 多字节命令 0000 0000b
#define OLED_CONTROLBYTE_CMD_SINGLE     (0x80) // 单字节命令 1000 0000b
#define OLED_CONTROLBYTE_DATA           (0x40) // 传输数据   0100 0000b

#define OLED_DISPLAY_ON                 (0xAF)
#define OLED_DISPLAY_OFF                (0xAE)

#define OLED_DISPGRAPH_SIZE             ((u16)(8*128))

#define OLED_I2C_RETRY_MAX              (9)

static const u8 init[] = {
    OLED_DISPLAY_OFF, // 关闭显示

    0xC8, // 设置COM扫描方向
    0x40, // 设置显示开始行
    0x81, // 设置对比度
    0xFF, // 对比度值
    0xA1, // 设置段重映射
    0xA6, // 设置正显
    0xA8, // 设置多路复用比
    0x3F, // 多路复用比值
    0xA4, // 全局显示开启

    0xD5, // 设置显示时钟分频
    0x80, // 分频值
    0xD9, // 设置预充电周期
    0x22, // 预充电周期值
    0xDA, // 设置COM硬件配置
    0x12, // COM硬件配置值
    0xDB, // 设置VCOMH
    0x30, // VCOMH值
    0x8D, // 设置电荷泵
    0x14, // 电荷泵值

    0xD3, // 设置显示偏移
    0x38, // 偏移值

    0xB0, // 给指针指回去（这里写几都不管用，画面都向上偏移了一页，即 8 px）
    0x00, // 给指针指回去
    0x10, // 给指针指回去（不知道为什么需要将偏移值设置为 56，也就是画面整体向下移一页才是正确的）

    0x20, // 设置内存地址模式
    0x00, // 设置水平寻址模式

    OLED_DISPLAY_ON,    // 显示开启
};

/**
 * @brief Configure the parameters of the display and initialize it.
 * 
 * @param oled OLED handle
 * @param cfg configuration parameters
 * @return true on success
 * @return false on failure (invalid parameters or i2c operation failure)
 */
bool i2c_oled_init(i2c_oled_t *oled, i2c_oled_cfg_t *cfg)
{
    CHECK_FALSE_RET(oled && cfg, false);
    CHECK_FALSE_RET(cfg->display_buf && cfg->i2c_mem_write_block && cfg->i2c_mem_write_dma, false);

    oled->is_init = false;
    oled->scr_is_on = true;
    oled->display_buf = cfg->display_buf;
    oled->i2c_mem_write_block = cfg->i2c_mem_write_block;
    oled->i2c_mem_write_dma = cfg->i2c_mem_write_dma;
    oled->i2c_send_reset = cfg->i2c_send_reset;
    oled->display_buf_size = cfg->display_buf_size;
    oled->width = cfg->width;
    oled->height = cfg->height;
    oled->i2c_addr = cfg->i2c_addr;

    oled->x_size_in_col = oled->width;
    oled->y_size_in_row = oled->height / 8;

    memset(oled->display_buf, 0x00, oled->display_buf_size);

    bool res = oled->i2c_mem_write_block(
        oled->i2c_addr, OLED_CONTROLBYTE_CMD_STREAM, (u8*)init, sizeof(init));

    oled->is_init = true;
    return res;
}

/**
 * @brief Clear the screen
 * 
 * @param oled OLED handle
 */
void i2c_oled_clear(i2c_oled_t *oled)
{
    CHECK_FALSE_RET_VOID(oled);
    CHECK_FALSE_RET_VOID(oled->is_init == true);
    CHECK_FALSE_RET_VOID(oled->display_buf);
    memset(oled->display_buf, 0x00, oled->display_buf_size);
}

/**
 * @brief Refresh the screen with the content of the display buffer.
 * 
 * @param oled OLED handle
 * @return true on success
 * @return false on failure (invalid parameters or i2c operation failure)
 */
bool i2c_oled_refresh(i2c_oled_t *oled)
{
    static u8 retry = 0;
    CHECK_FALSE_RET(oled, false);
    CHECK_FALSE_RET(oled->is_init == true, false);
    CHECK_FALSE_RET(oled->display_buf && oled->i2c_mem_write_dma, false);
    if (oled->i2c_mem_write_dma(oled->i2c_addr, OLED_CONTROLBYTE_DATA, oled->display_buf, oled->display_buf_size)) {
        retry = 0;
        return true;
    }
    if (retry < OLED_I2C_RETRY_MAX) {
        retry++;
        return false;
    }
    retry = 0;
    if (oled->i2c_send_reset) {
        oled->i2c_send_reset();
        SLOGI(TAG, "I2C bus reset");
    }
    return false;
}

/**
 * @brief Turn the display on or off.
 * 
 * @param oled OLED handle
 * @param on true to turn on, false to turn off
 * @return true on success
 * @return false on failure (invalid parameters or i2c operation failure)
 */
bool i2c_oled_scr_on_off(i2c_oled_t *oled, bool on)
{
    CHECK_FALSE_RET(oled, false);
    CHECK_FALSE_RET(oled->is_init == true, false);
    CHECK_FALSE_RET(oled->i2c_mem_write_block, false);
    u8 cmd = on ? OLED_DISPLAY_ON : OLED_DISPLAY_OFF;
    bool result = oled->i2c_mem_write_block(oled->i2c_addr, OLED_CONTROLBYTE_CMD_SINGLE, &cmd, 1);
    if (result) {
        oled->scr_is_on = on;
    }
    return result;
}

/**
 * @brief Show a character on the display using a specified font.
 * @note  0.96 inch OLED (128*64) has 128 columns and 8 rows.
 * 
 * @param oled OLED handle
 * @param row Y coordinate in rows (1 column = 8 pixels), range 0 ~ (height / 8 - 1)
 * @param col X coordinate in columns, range 0 ~ (width - 1)
 * @param font Font to use
 * @param num Character index in the font
 */
void i2c_oled_show_char(i2c_oled_t *oled, u8 row, u8 col, const i2c_oled_font_t *font, u32 num)
{
    CHECK_FALSE_RET_VOID(oled && font);
    CHECK_FALSE_RET_VOID(font->data);
    CHECK_FALSE_RET_VOID(font->width > 0 && font->height > 0);
    CHECK_FALSE_RET_VOID(num < font->size);
    CHECK_FALSE_RET_VOID(col < oled->x_size_in_col && row < oled->y_size_in_row);

    u16 font_x_size_in_col = font->width;
    u16 font_y_size_in_row = font->height / 8;
    u16 font_char_size = font_x_size_in_col * font_y_size_in_row;

    /* 防止 memcpy 写越界 */
    CHECK_FALSE_RET_VOID((u16)col + font_x_size_in_col <= oled->x_size_in_col);
    CHECK_FALSE_RET_VOID((u16)row + font_y_size_in_row <= oled->y_size_in_row);

    u8 *p = oled->display_buf + row * oled->x_size_in_col + col;
    for (u16 i = 0; i < font_y_size_in_row; i++) {
        CHECK_FALSE_RET_VOID(p >= oled->display_buf);
        CHECK_FALSE_RET_VOID(p + font_x_size_in_col <= oled->display_buf + oled->display_buf_size);
        memcpy(p, font->data + (num * font_char_size) + (i * font_x_size_in_col), font_x_size_in_col);
        p += oled->x_size_in_col;
    }
}
