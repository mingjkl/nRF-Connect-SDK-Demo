## Introduction
This demo showcases a UART echo demo implemented using the nrfx library. The demo will return the data received on the serial port as it is.

## Key Code
* Add UART configuration in `prj.conf`:
    ```
    CONFIG_NRFX_UARTE0=y
    ```
    In this demo, we are using UART0. Note that the log output is also using UART0, but there should be no conflicts.

* Add the following header files in `main.c`:
    ```
    #include <nrfx_uarte.h>
    #include <hal/nrf_gpio.h>
    ```
    We use `nrf_gpio.h` here to use `NRF_GPIO_PIN_MAP`.

* Define a UARTE instance:
    ```
    static nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(UARTE_INST_IDX);
    ```
    Where `UARTE_INST_IDX` is defined as `#define UARTE_INST_IDX 0`.

* Define the UART configuration structure:
    ```
    nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG(UART_TX_PIN, UART_RX_PIN);
    ```
    Here, we use the default configuration, where `UART_TX_PIN` and `UART_RX_PIN` are the TX and RX pins of UART. In this case, we use `NRF_GPIO_PIN_MAP(0, 6)` and `NRF_GPIO_PIN_MAP(0, 8)`. These pins are the debug UART pins on the nRF52840 Development Kit.

* Initialize UART:
    ```
    nrfx_uarte_init(&uarte_instance, &uarte_config, uarte_event_handler);
    ```

* Implement the UART interrupt handler:
    ```c
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
    Here, we use `nrfx_uarte_rx` to configure a receive data buffer `rx_data` with a length of 1. This means that every time one byte of data is received, it will trigger an interrupt. The received data is stored in the `rx_buffer`, and when the end symbol is received, the data in the `rx_buffer` is returned as it is.

 * To make the interrupt effective, we need to introduce the interrupt handling function:
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(UARTE_INST_IDX)), IRQ_PRIO_LOWEST,
                       NRFX_UARTE_INST_HANDLER_GET(UARTE_INST_IDX), 0);
    ```

 * After initializing UARTE, we also need to configure the receive data buffer to correctly receive data when the first interrupt is triggered:
    ```
    nrfx_uarte_rx(&uarte_instance, &rx_data, sizeof(rx_data));
    ```

* Compile and flash the program, open a serial debugging tool, set the baud rate to `115200`, and send data. You should see the data being echoed back as it is entered.