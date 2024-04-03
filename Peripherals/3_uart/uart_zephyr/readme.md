## 简介
这是一个使用`Zephyr RTOS API`实现的`UART`的`echo`程序。程序会将接收到的字符通过`UART`发送出去。

## 关键代码
* 在`prj.conf`中加入`kconfig`配置
    ```c
    CONFIG_SERIAL=y
    CONFIG_UART_ASYNC_API=y
    ```

* 新建`.overlay`文件，配置`UART`的波特率、引脚等
    ```c
    &uart0 {
	status = "okay";
	current-speed = <115200>;
    };

    &uart0_default {
        group1 {
            psels = <NRF_PSEL(UART_TX, 0, 6)>, <NRF_PSEL(UART_RTS, 0, 5)>;
        };

        group2 {
            psels = <NRF_PSEL(UART_RX, 0, 8)>, <NRF_PSEL(UART_CTS, 0, 7)>;
        };
    };

    ```

* 在`main.c`中加入必要的头文件
    ```c
    #include <zephyr/drivers/uart.h>
    #include <zephyr/device.h>
    #include <zephyr/devicetree.h>
    ```

* 定义`UART`的设备
    ```c
    const struct device *uart= DEVICE_DT_GET(DT_NODELABEL(uart0));
    ```

* 判定`UART`设备是否可用
  ```c
  if (!device_is_ready(uart)){
		LOG_ERR("UART device not ready\r\n");
		return 1 ;
	}
	else{
		LOG_INF("UART device ready\r\n");
	}
  ```

* 回调函数设置
  ```c
  ret = uart_callback_set(uart, uart_cb, NULL);
	if (ret) {
		LOG_ERR("Cannot set callback");
		return 1;
	}
  ```

* 设置`UART`的接收缓冲区和使能接收
  ```c
  ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT_US);
	if (ret) {
		LOG_ERR("Cannot enable rx");
		return 1;
	}
  ```
  这里配置了接收缓冲区的大小和接收超时时间。其中`rx_buf`数组是用来存放接收到的数据的。`RECEIVE_TIMEOUT_US`是接收超时时间,即断帧超时时间，接收到一定数据后，`RECEIVE_TIMEOUT_US`时间内没有接收到数据，则认为接收完成。触发`UART_RX_RDY`事件,进入回调函数。这里`RECEIVE_TIMEOUT_US`的单位是微秒。如设置过小，可能会导致接收不完整。在实际应用中，需要根据实际情况进行调整。

* 回调函数实现
    ```c
    static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
    {
    switch (evt->type) {

        case UART_RX_RDY:
            LOG_HEXDUMP_DBG(evt->data.rx.buf, evt->data.rx.len, "data: ");

            for(uint16_t i = 0; i < evt->data.rx.len; i++){
                tx_buf[i] = evt->data.rx.buf[i];
            }

            tx_buf_len = evt->data.rx.len;

            uart_rx_disable(dev);

            break;
        case UART_RX_DISABLED:
            uart_rx_enable(dev ,rx_buf,sizeof rx_buf,RECEIVE_TIMEOUT_US);
            break;
        default:
            break;
        }
    }
    ```
    触发`UART_RX_RDY`事件，进入回调函数。这里将`evt->data.rx.buf`中的数据复制到`tx_buf`中，并设置`tx_buf_len`为接收到的数据长度。使用`uart_rx_disable`函数关闭接收，`evt->data.rx.buf`中的数据会被清空。`UART_RX_DISABLED`事件触发后，再次使能接收,准备接收新的数据。

* 发送数据
    ```c
    int uart_data_send(const struct device *dev, const uint8_t *buf, uint8_t len)
    {
        int ret;

        ret = uart_tx(dev, buf, len, SYS_FOREVER_MS);
        if (ret) {
            LOG_ERR("Failed to send data over UART");
            return ret;
        }

        return 0;
    }
    ```
    这里将`uart_tx`函数重新封装，方便调用。

* 数据`echo`的实现
    ```c
    while (1) {
        if(tx_buf_len > 0){
            uart_data_send(uart, tx_buf, tx_buf_len);
            tx_buf_len = 0;
        }
    }
    ```
    这里判断`tx_buf`中是否有数据，如果有数据，则发送出去。

* 编译并烧录程序
* 通过串口调试助手，打开串口，设置波特率为115200，发送数据，即可看到接收到的数据被原样发送出去.
