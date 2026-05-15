
#include "cn1608.h"
#include "sys_err.h"
#include "display.h"

// The order of characters in this string should match the order of the corresponding font data in the cn1608 array.
static const char cn1608_cnch[] = "月日";

static const u8 Cn1608Char[] =
{
    0x00,0x00,0xE0,0x20,0x20,0x20,0x20,0xE0,
    0x00,0x80,0x7F,0x09,0x09,0x09,0x89,0xFF,  /* "月" 0 */
    0x00,0x00,0xE0,0x20,0x20,0x20,0x20,0xE0,
    0x00,0x00,0xFF,0x42,0x42,0x42,0x42,0xFF,  /* "日" 1 */
};

static const i2c_oled_font_t font_cn1608 = {
    .width = 8,
    .height = 16,
    .size = 2,
    .data = Cn1608Char
};

/**
 * @brief Show a string of Chinese characters on the OLED display using the cn1608 font.
 * @note  Each character should be encoded in UTF-8 and take 3 bytes.
 * 
 * @param row y coordinate in rows (1 row = 8 pixels), range 0 ~ (height / 8 - 1)
 * @param col x coordinate in cols, range 0 ~ (width - 1)
 * @param str String of Chinese characters to display
 */
void oled_show_cn1608_str(u8 row, u8 col, char const *str)
{
    CHECK_FALSE_RET_VOID(str);
    CHECK_FALSE_RET_VOID(row < oled.y_size_in_row && col < oled.x_size_in_col);
    u16 font_x_size_in_col = font_cn1608.width;
    u16 font_y_size_in_row = font_cn1608.height / 8;

    while (*str)
    {
        // Find the index of the current character in the cnch string
        const char *ptr = cn1608_cnch;
        int index = 0;
        while (*ptr)
        {
            if (strncmp(ptr, str, 3) == 0) // compare 3 bytes for UTF-8 character
            {
                break;
            }
            ptr += 3; // move to the next character (3 bytes for UTF-8)
            index++;
        }
        CHECK_FALSE_RET_VOID(*ptr); // character not found in cnch string

        if (col + font_x_size_in_col > oled.x_size_in_col)
        {
            col = 0;
            row += font_y_size_in_row;
            if (row + font_y_size_in_row > oled.y_size_in_row)
            {
                break;
            }
        }
        i2c_oled_show_char(&oled, row, col, &font_cn1608, index);
        col += font_x_size_in_col;
        str += 3; // move to the next UTF-8 character (3 bytes)
    }
}
