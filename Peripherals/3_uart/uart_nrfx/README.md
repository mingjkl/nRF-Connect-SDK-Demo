## 简介
这个demo演示的是使用nrfx库实现的uart echo demo，demo会将串口收到的数据原样返回。

## 关键代码
* 在pro.conf中添加串口配置
    ```
    CONFIG_NRFX_UARTE0=y
    ```
    这个demo中我们使用的是uart0，注意log的输出也是使用的uart0，不过这里没有发现冲突。

* 在main.c中添加头文件
    ```
    #include <nrfx_uarte.h>
    #include <hal/nrf_gpio.h>
    ```
    这里面使用`nrf_gpio.h`是为了使用`NRF_GPIO_PIN_MAP`;

* 定义一个uarte的实例
    ```
    static nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(UARTE_INST_IDX);
    ```
    其中`UARTE_INST_IDX`使用的`#define UARTE_INST_IDX 0`

* 定义uart的配置结构体
    ```
    nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG(UART_TX_PIN, UART_RX_PIN);
    ```
    这里使用的是默认配置，其中`UART_TX_PIN`和`UART_RX_PIN`分别是uart的tx和rx引脚，这里使用的是`NRF_GPIO_PIN_MAP(0, 6)`和`NRF_GPIO_PIN_MAP(0, 8)`,这两个引脚是nrf52840dk上的debug串口引脚。

* 初始化uart
    ```
    nrfx_uarte_init(&uarte_instance, &uarte_config, uarte_event_handler);
    ```

* 补充uart的中断处理函数的实现
    ```
    #define RX_BUFFER_SIZE 256
    #define RX_END_SYMBOL '\n'

    uint8_t rx_buffer[RX_BUFFER_SIZE];
    uint16_t rx_buffer_index = 0;

    uint8_t rx_data;

    static void uart_handler(nrfx_uarte_event_t const * p_event, void * p_context)
    {
        switch (p_event->type) {
            case NRFX_UARTE_EVT_TX_DONE:
                break;
            case NRFX_UARTE_EVT_RX_DONE:
                nrfx_uarte_rx(&uarte_instance, &rx_data, sizeof(rx_data));
                rx_buffer[rx_buffer_index] = rx_data;

                rx_buffer_index++;

                if (rx_data == RX_END_SYMBOL) {
                    nrfx_uarte_tx(&uarte_instance, rx_buffer, rx_buffer_index);
                    rx_buffer_index = 0;
                }

                if (rx_buffer_index >= RX_BUFFER_SIZE) {
                    rx_buffer_index = 0;
                }
                break;
            case NRFX_UARTE_EVT_ERROR:
                break;
            default:
                break;
	}
    ```
    这里我们通过`nrfx_uarte_rx`配置的接收数据缓冲区`rx_data`长度为1，所以每次接收到一个字节的数据后，就会触发一次中断，然后将数据存入`rx_buffer`中，当接收到的数据为结束符号时，就将`rx_buffer`中的数据原样返回

 * 为了让中断生效，我们还需要引入中断处理函数
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(UARTE_INST_IDX)), IRQ_PRIO_LOWEST,
                       NRFX_UARTE_INST_HANDLER_GET(UARTE_INST_IDX), 0);
    ```

 * 对了，在uarte初始化后，还需要配置一下接收数据的缓冲区，以在第一次中断触发时，能够正确的接收数据
    ```
    nrfx_uarte_rx(&uarte_instance, &rx_data, sizeof(rx_data));
    ```

* 编译并烧录程序，打开串口调试工具，设置波特率为`115200`，发送数据，就可以看到数据被原样返回了；