## Introduction
This is an `echo` program for `UART` implemented using the `Zephyr RTOS API`. The program will echo back characters received via `UART`.

## Key Code
* Add `kconfig` settings in `prj.conf`
    ```c
    CONFIG_SERIAL=y
    CONFIG_UART_ASYNC_API=y
    ```

* Create a `.overlay` file to configure the `UART` baud rate, pins, etc.
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

* Include necessary headers in `main.c`
    ```c
    #include <zephyr/drivers/uart.h>
    #include <zephyr/device.h>
    #include <zephyr/devicetree.h>
    ```

* Define the `UART` device
    ```c
    const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
    ```

* Check if the `UART` device is ready
  ```c
  if (!device_is_ready(uart)){
        LOG_ERR("UART device not ready\r\n");
        return 1;
    }
    else{
        LOG_INF("UART device ready\r\n");
    }
  ```

* Set up callback function
  ```c
  ret = uart_callback_set(uart, uart_cb, NULL);
    if (ret) {
        LOG_ERR("Cannot set callback");
        return 1;
    }
  ```

* Set `UART` receive buffer and enable reception
  ```c
  ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT_US);
    if (ret) {
        LOG_ERR("Cannot enable rx");
        return 1;
    }
  ```
  This configures the size of the receive buffer and the receive timeout. The `rx_buf` array stores the received data. `RECEIVE_TIMEOUT_US` is the receive timeout, i.e., the frame break timeout. If no data is received within `RECEIVE_TIMEOUT_US` after receiving a certain amount of data, it is assumed that reception is complete, triggering the `UART_RX_RDY` event and entering the callback function. `RECEIVE_TIMEOUT_US` is in microseconds. Setting it too low may result in incomplete reception. Adjust according to actual conditions in practical applications.

* Implement callback function
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
                uart_rx_enable(dev, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT_US);
                break;
            default:
                break;
        }
    }
    ```
    When the `UART_RX_RDY` event is triggered, enter the callback function. Here, data from `evt->data.rx.buf` is copied to `tx_buf`, and `tx_buf_len` is set to the length of received data. Use the `uart_rx_disable` function to stop reception; data in `evt->data.rx.buf` will be cleared. After the `UART_RX_DISABLED` event is triggered, re-enable reception to prepare for new data.

* Data sending
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
    Here, the `uart_tx` function is encapsulated for convenience.

* Implement data `echo`
    ```c
    while (1) {
        if(tx_buf_len > 0){
            uart_data_send(uart, tx_buf, tx_buf_len);
            tx_buf_len = 0;
        }
    }
    ```
    Here, it checks if there is data in `tx_buf`; if there is, it sends it out.

* Compile and flash