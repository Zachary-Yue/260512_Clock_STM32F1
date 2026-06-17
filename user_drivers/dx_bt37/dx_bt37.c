
#include "dx_bt37.h"
#include <stdarg.h>
#include "debug.h"
#include "sys_err.h"
#include "user_phr.h"

#define TAG "DX_BT37"

#define DX_BT37_DELAY(ms) LL_mDelay(ms)

static const uint32_t dx_bt37_baud_rates[DX_BT37_BAUD_MAX] = {
    2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1000000
};

#define DX_BT37_VSNPRINTF_BUF_SIZE 1000
#define DX_BT37_WAIT_MAX 1000

static bool dx_bt37_test_com(dx_bt37_t *bt37);
static bool dx_bt37_lp(dx_bt37_t *bt37);
static bool dx_bt37_set_name(dx_bt37_t *bt37);

// 临时用宏定义先分个类，后续会集成到初始化 config 里。
#define PRIV_BAUD_AUTO_DETECT 0

/**
 * @brief DX_BT37 初始化，自动波特率检测。可以用来 reinit。
 * 
 * @note tx_buf（发送缓冲区）是一次发送数据的临时存储空间，DX_BT37 会从 tx_fifo 中读取数据到 tx_buf 中，
 *   然后通过 UART 发送出去。tx_buf 的大小决定了每次能发送的最大数据量，如果 tx_buf 太小可能会导致发送效率低下，
 *   因为需要频繁地从 tx_fifo 中读取数据并发送。建议 tx_buf_size 至少为 64 字节，以适应大多数应用场景。
 * 
 * @note 
 * 
 * @param bt37   BT37 句柄
 * @param config 初始化配置，用户需要配置每一个字段。
 */
bool dx_bt37_init(dx_bt37_t *bt37, dx_bt37_config_t *config)
{
    CHECK_FALSE_RET(bt37 && config, false);
    CHECK_FALSE_RET(
        config->send_attr.uart_send && config->send_attr.tx_buf && config->send_attr.tx_buf_size &&
        config->send_attr.tx_fifo_buf && config->send_attr.tx_fifo_buf_size && config->recv_attr.uart_recv, false);

    LOGI(TAG, "Starting initialize DX_BT37...");

    // Set default values and copy configuration
    memset(bt37, 0, sizeof(dx_bt37_t));
    bt37->uart_send = config->send_attr.uart_send;
    bt37->uart_recv = config->recv_attr.uart_recv;
    bt37->baud = DX_BT37_BAUD_115200; // 默认波特率，后续可能会被自动检测覆盖
    bt37->tx_buf = config->send_attr.tx_buf;
    bt37->tx_buf_size = config->send_attr.tx_buf_size;
    user_fifo_init(&bt37->tx_fifo, config->send_attr.tx_fifo_buf, config->send_attr.tx_fifo_buf_size);

    #if PRIV_BAUD_AUTO_DETECT
        LOGI(TAG, "Starting auto baud rate detection...");
        uart_abort(bt37->uart_send, bt37->uart_recv); // 确保 UART 处于空闲状态

        // auto baud rate detection
        u8 baud_index = DX_BT37_BAUD_2400;
        while (baud_index < DX_BT37_BAUD_MAX) {
            LOGI(TAG, "Testing baud rate: %lu", dx_bt37_baud_rates[baud_index]);
            uart_change_baud(bt37->uart_send->Instance, dx_bt37_baud_rates[baud_index]);
            if (dx_bt37_test_com(bt37))
            {
                LOGI(TAG, "Baud rate detected: %lu", dx_bt37_baud_rates[baud_index]);
                bt37->baud = baud_index;
                bt37->is_init = true;
                break;
            }
            baud_index++;
            debug_send();   // 再不发的话 debug 缓冲区要被写满了，无奈之举
        }

        bt37->is_init = (baud_index < DX_BT37_BAUD_MAX);
    #else
        uart_change_baud(bt37->uart_send->Instance, dx_bt37_baud_rates[DX_BT37_BAUD_115200]);
        bt37->is_init = true;
    #endif

    CHECK_FALSE_RET_LOG(bt37->is_init, false, TAG,
        "Failed to detect baud rate. Init failed.");

    if (dx_bt37_lp(bt37)) {
        LOGI(TAG, "Open BT37 LP mode successfully.");
    }
    else {
        LOGW(TAG, "Failed to open BT37 LP mode.");
    }

    dx_bt37_set_name(bt37); // Set BT37 name to BT_NAME

    bt37->rx_enabled = config->recv_attr.rx_buf && config->recv_attr.rx_buf_size &&
        config->recv_attr.rx_proc_buf && config->recv_attr.rx_proc_buf_size;
    if (bt37->rx_enabled) {
        bt37->rx_buf = config->recv_attr.rx_buf;
        bt37->rx_buf_size = config->recv_attr.rx_buf_size;
        bt37->rx_proc_buf = config->recv_attr.rx_proc_buf;
        bt37->rx_proc_buf_size = config->recv_attr.rx_proc_buf_size;
        bt37->dx_bt37_rx_data_proc = config->recv_attr.dx_bt37_rx_data_proc;
        uart_receive_start(bt37->uart_recv, bt37->rx_buf, true, bt37->rx_buf_size);
    }

    LOGI(TAG, "DX_BT37 initialized successfully.");
    return true;
}

/**
 * @brief DX_BT37 修改波特率。若修改失败会恢复到原来的波特率。
 * 
 * @param bt37  初始化过的 BT37 句柄
 * @param baud  要修改的波特率，必须是 dx_bt37_baud_t 枚举类型中的值
 * @return bool true 修改成功，false 修改失败（可能是通信失败或者波特率无效）
 * 
 * @note 由于需要阻塞等待模块重启和通信测试，这个函数大概需要 1 秒钟左右的时间才能完成。
 */
bool dx_bt37_change_baud(dx_bt37_t *bt37, dx_bt37_baud_t baud)
{
    CHECK_FALSE_RET(bt37 && baud < DX_BT37_BAUD_MAX, false);
    CHECK_FALSE_RET(bt37->is_init, false);

    LOGI(TAG, "Start changing baud rate to: %lu...", dx_bt37_baud_rates[baud]);
    debug_send();

#define TMP_BUF_SIZE 4
    u8 old_baud_index = bt37->baud;
    u8 tmp_buf[TMP_BUF_SIZE];

    dx_bt37_send_all(bt37); // 确保命令发送完成
    dx_bt37_printf(bt37, "AT+BAUD%X\r\n", baud + 1);
    dx_bt37_send_all(bt37); // 确保命令发送完成
    DX_BT37_DELAY(50); // 等待模块处理命令
    dx_bt37_printf(bt37, "AT+RESET\r\n");
    dx_bt37_send_all(bt37); // 确保命令发送完成

    uart_abort(bt37->uart_send, bt37->uart_recv); // 确保 UART 处于空闲状态
    uart_change_baud(bt37->uart_send->Instance, dx_bt37_baud_rates[baud]);
    DX_BT37_DELAY(800); // 等待模块开机以及 UART 稳定

    // 接收可能的残余数据，避免干扰后续通信
    uart_receive_start(bt37->uart_recv, tmp_buf, false, TMP_BUF_SIZE);
    DX_BT37_DELAY(20); // 等待数据接收完成

    if (dx_bt37_test_com(bt37)) {
        bt37->baud = baud;
        LOGI(TAG, "Baud rate changed successfully to: %lu", dx_bt37_baud_rates[baud]);
    } else {
        // 恢复到原来的波特率
        uart_abort(bt37->uart_send, bt37->uart_recv); // 确保 UART 处于空闲状态
        uart_change_baud(bt37->uart_send->Instance, dx_bt37_baud_rates[old_baud_index]);
        LOGE(TAG, "Failed to change baud rate to: %lu, reverted to: %lu",
            dx_bt37_baud_rates[baud], dx_bt37_baud_rates[old_baud_index]);
        return false;
    }

    if (bt37->rx_enabled) {
        uart_receive_start(bt37->uart_recv, bt37->rx_buf, true, bt37->rx_buf_size);
    }
    return true;
}

/**
 * @brief DX_BT37 发送数据。
 * 
 * @param bt37  BT37 句柄
 * @param format 格式化字符串
 * @param ...    可变参数，支持 printf 的格式化输出
 */
void dx_bt37_printf(dx_bt37_t *bt37, const char *format, ...)
{
    CHECK_FALSE_RET_VOID(bt37);
    CHECK_FALSE_RET_VOID(bt37->is_init);
    u8 printf_buf[DX_BT37_VSNPRINTF_BUF_SIZE];
    va_list args;
    va_start(args, format);
    int len = vsnprintf((char *)printf_buf, DX_BT37_VSNPRINTF_BUF_SIZE, format, args);
    va_end(args);
    if (len <= 0) return;
    if (len > DX_BT37_VSNPRINTF_BUF_SIZE) len = DX_BT37_VSNPRINTF_BUF_SIZE;
    user_fifo_write(&bt37->tx_fifo, printf_buf, len);
}

/**
 * @brief DX_BT37 发送数据，直接发送二进制数据，不进行格式化。
 * 
 * @param bt37  BT37 句柄
 * @param data  要发送的二进制数据
 * @param len   数据长度
 */
void dx_bt37_send_raw(dx_bt37_t *bt37, const u8 *data, u16 len)
{
    CHECK_FALSE_RET_VOID(bt37);
    CHECK_FALSE_RET_VOID(bt37->is_init);
    user_fifo_write(&bt37->tx_fifo, data, len);
}

/**
 * @brief BT37 发送数据任务，放在主循环中。
 * 
 * @param bt37 BT37 句柄
 */
void dx_bt37_send(dx_bt37_t *bt37)
{
    CHECK_FALSE_RET_VOID(bt37->uart_send);
    CHECK_FALSE_RET_VOID(bt37->is_init);
    CHECK_FALSE_RET_VOID(!uart_is_sending(bt37->uart_send));
    CHECK_FALSE_RET_VOID(user_fifo_get_used_length(&bt37->tx_fifo));
    u16 len_to_send = user_fifo_get_used_length(&bt37->tx_fifo);
    if (len_to_send > bt37->tx_buf_size) len_to_send = bt37->tx_buf_size; // 限制一次发送的数据量
    user_fifo_read(&bt37->tx_fifo, bt37->tx_buf, len_to_send, USER_FIFO_READ_AND_CLEAN);
    uart_send_start(bt37->uart_send, bt37->tx_buf, len_to_send);
}

/**
 * @brief 将 BT37 发送缓冲区中的所有数据发送出去，直到发送完成或者没有数据了。这个函数会阻塞等待发送完成，慎用。
 * 
 * @param bt37 BT37 句柄
 */
void dx_bt37_send_all(dx_bt37_t *bt37)
{
    CHECK_FALSE_RET_VOID(bt37);
    CHECK_FALSE_RET_VOID(bt37->is_init);
    uart_send_wait(bt37->uart_send, DX_BT37_WAIT_MAX); // 等待当前发送完成
    while (user_fifo_get_used_length(&bt37->tx_fifo) > 0) {
        dx_bt37_send(bt37);
        uart_send_wait(bt37->uart_send, DX_BT37_WAIT_MAX); // 等待当前发送完成
    }
}

/**
 * @brief 在接收结束的中断中调用。将接收到的数据拷贝到处理缓冲区中等待处理。
 * 
 * @param bt37 BT37 句柄
 * @param len  本次接收到的长度
 */
void dx_bt37_rx_cplt_handler(dx_bt37_t *bt37, u16 len)
{
    CHECK_FALSE_RET_VOID(bt37 && len);
    CHECK_FALSE_RET_VOID(bt37->is_init && bt37->rx_enabled);

    // 将接收到的数据拷贝到处理缓冲区中
    u16 proc_len = min(len, min(bt37->rx_buf_size, bt37->rx_proc_buf_size));
    memset(bt37->rx_proc_buf, 0, bt37->rx_proc_buf_size);
    memcpy(bt37->rx_proc_buf, bt37->rx_buf, proc_len);
    bt37->rx_received_len = proc_len;
    
    // 启动下一次的 DMA 接收
    // 设置数据待处理标志位
    bt37->rx_wait_for_proc = true;
}

/**
 * @brief 在 main 循环调用。当发现有数据待处理时，处理数据。
 * 
 * @param bt37 BT37 句柄
 */
void dx_bt37_recv(dx_bt37_t *bt37)
{
    CHECK_FALSE_RET_VOID(bt37);
    CHECK_FALSE_RET_VOID(bt37->is_init && bt37->rx_enabled && bt37->dx_bt37_rx_data_proc);
    CHECK_FALSE_RET_VOID(bt37->rx_wait_for_proc);
    bt37->rx_wait_for_proc = false;
    bt37->dx_bt37_rx_data_proc(bt37, bt37->rx_proc_buf, bt37->rx_received_len);
}

/**
 * @brief 测试 BT37 的通信测试
 * 
 * @param bt37 BT37 句柄
 * @param test_string 要发送的字符串
 * @param recv_buf 接收回应的缓冲区，给 NULL 表示不接收回应
 * @param test_recv_buf_size 接收缓冲区的大小，给 0 表示不接收回应
 * @return true 字符串成功发送给 BT37，并且在启用接收时成功接收到回应
 * @return false 发送或者接收失败
 * 
 * @note 虽然可以不使能接收，但是如果会有消息从模块发来的话，还是建议接收一下，避免残余数据干扰后面的接收。
 */
static bool dx_bt37_test(
    dx_bt37_t *bt37,
    const char *test_string,
    char *recv_buf,
    u16 test_recv_buf_size)
{
    CHECK_FALSE_RET(bt37 && test_string, false);
    CHECK_FALSE_RET(bt37->uart_send, false);
    bool en_recv = bt37->uart_recv && recv_buf && test_recv_buf_size > 0;

    if (en_recv) {
        memset(recv_buf, 0, test_recv_buf_size);
        uart_abort(bt37->uart_send, bt37->uart_recv); // 确保 UART 处于空闲状态
        // uart_receive_start(bt37->uart_recv, (u8*)recv_buf, false, test_recv_buf_size);
        // uart_recv_wait(bt37->uart_recv, 5); // 等待可能的残余数据接收完成，避免干扰后续通信
    }

    CHECK_FALSE_RET(uart_send_start(bt37->uart_send,
        (u8*)test_string, strlen(test_string)), false);
    if (uart_send_wait(bt37->uart_send, 30) == false) {
        uart_abort(bt37->uart_send, bt37->uart_recv);
        return false;
    }

    if (en_recv) {
        CHECK_FALSE_RET(uart_receive_start(bt37->uart_recv,
            (u8*)recv_buf, false, test_recv_buf_size), false);
        if (uart_recv_wait(bt37->uart_recv, 30) == false) {
            uart_abort(bt37->uart_send, bt37->uart_recv);
            recv_buf[test_recv_buf_size - 1] = '\0';
            return false;
        }
        recv_buf[test_recv_buf_size - 1] = '\0';
    }
    return true;
}

#define BT_NAME "YourLovelyClock"
#define BT_NAME_CMD "AT+NAME" BT_NAME "\r\n"
#define TEST_COM_STRING "AT\r\n"
#define TEST_COM_RECV_STRING "OK\r\n"
#define LP_CMD "AT+PWRM0\r\n"
#define LP_RECV_STRING "+PWRM=0"
#define NM_CMD "AT+PWRM1\r\n"
#define NM_RECV_STRING_1 "+PWRM=1"
#define NM_RECV_STRING_2 "OK\r\n"
#define TEST_RECV_BUF_SIZE 32
#define DISC_CMD "AT+DISC\r\n"

/**
 * @brief 给 BT37 发送 AT\r\n，通过判断回复是否为 OK\r\n 来判断模块收发是否正常。
 * 
 * @param bt37  BT37 句柄
 * @return bool true 通信正常，false 通信异常
 */
static bool dx_bt37_test_com(dx_bt37_t *bt37)
{
    char test_recv_string_buf[TEST_RECV_BUF_SIZE];
    // if (dx_bt37_test(bt37, NM_CMD, test_recv_string_buf, TEST_RECV_BUF_SIZE)) {
    //     if (begins_with_str((char*)test_recv_string_buf, NM_RECV_STRING_1) ||
    //         begins_with_str((char*)test_recv_string_buf, NM_RECV_STRING_2)) {
    //         return true;
    //     }
    // }
    if (dx_bt37_test(bt37, TEST_COM_STRING, test_recv_string_buf, TEST_RECV_BUF_SIZE)) {
        if (begins_with_str((char*)test_recv_string_buf, TEST_COM_RECV_STRING)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 使 BT37 进入低功耗模式，发送 AT+PWRM0 命令。
 * 
 * @param bt37 BT37 句柄
 * @return true 成功发送命令
 * @return false 发送失败
 */
static bool dx_bt37_lp(dx_bt37_t *bt37)
{
    char lp_recv_buf[TEST_RECV_BUF_SIZE];
    return dx_bt37_test(bt37, LP_CMD, lp_recv_buf, TEST_RECV_BUF_SIZE);
}

/**
 * @brief 设置 BT37 的蓝牙名称，发送 AT+NAME 命令。
 * 
 * @param bt37 BT37 句柄
 * @param name 要设置的蓝牙名称，建议长度不超过 28 字节，否则可能会被模块截断
 * @return true 成功发送命令并且接收到回应
 * @return false 发送命令失败或者接收回应失败
 */
static bool dx_bt37_set_name(dx_bt37_t *bt37)
{
    char name_recv_buf[TEST_RECV_BUF_SIZE];
    return dx_bt37_test(bt37, BT_NAME_CMD, name_recv_buf, TEST_RECV_BUF_SIZE);
}

/**
 * @brief 断开 BT37 连接，发送 AT+DISC 命令。这个函数会阻塞等待发送完成，慎用。
 * 
 * @param bt37 BT37 句柄
 */
void dx_bt37_disconnect(dx_bt37_t *bt37)
{
    CHECK_FALSE_RET_VOID(bt37);
    CHECK_FALSE_RET_VOID(bt37->is_init);
    dx_bt37_send_all(bt37); // 确保发送完成
    LL_mDelay(20); // 主动分包，避免 AT+DISC 命令和之前的命令一起发送导致模块不响应
    dx_bt37_printf(bt37, DISC_CMD);
    dx_bt37_send_all(bt37); // 确保发送完成
}
