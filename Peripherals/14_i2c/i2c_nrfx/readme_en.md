## Introduction
This is a demo of using the `nrfx_twi` driver for I2C communication. In this demo, the `nrfx_twi` driver is validated by performing read and write operations on an AT24C02 chip.

## Key Code
* Add the `twi` configuration to `prj.conf`
    ```
    CONFIG_NRFX_TWI0=y
    CONFIG_LOG=y
    ```
    Here, we are using `twi0` and enabling the `log` configuration for printing debug information. However, we won't cover the details of the `log` in this context.
* Include necessary header files in `main.c`
    ```c
    #include <nrfx_twi.h>
    #include <hal/nrf_gpio.h>
    #include <zephyr/logging/log.h>
    ```
* Define macros for the I2C pins
    ```c
    #define SDA_PIN NRF_GPIO_PIN_MAP(0, 31)
    #define SCL_PIN NRF_GPIO_PIN_MAP(0, 30)
    ```
* Define an instance of the `twi` driver
    ```c
    static const nrfx_twi_t twi = NRFX_TWI_INSTANCE(0);
    ```
* Define the configuration for the `twi` driver
    ```c
    nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG(SCL_PIN, SDA_PIN);
    ```
    Here, we are using the default configuration.
* Initialize the `twi` driver
    ```c
    nrfx_twi_init(&twi, &config, twi_handler, NULL);
    ```
* Implement the interrupt handler for `twi`
    ```c
    void twi_handler(nrfx_twi_evt_t const * p_event, void * p_context) 
    {
        switch (p_event->type) {
            case NRFX_TWI_EVT_DONE:
                /* Transfer completed */
                break;
            case NRFX_TWI_EVT_ADDRESS_NACK:
                /* NACK received after sending the address */
                break;
            case NRFX_TWI_EVT_DATA_NACK:
                /* NACK received after sending a data byte */
                break;
            default:
                break;
        }
    }
    ```
* Connect the interrupt
    ```c
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TWI_INST_GET(0)), IRQ_PRIO_LOWEST,
                       NRFX_TWI_INST_HANDLER_GET(0), 0);
    ```
* Enable the `twi` driver
    ```c
    nrfx_twi_enable(&twi);
    ```
* Data interaction using I2C
  * Data interaction is mainly done using the `nrfx_twi_xfer` function, which takes three parameters:
    * `rfx_twi_t const *p_instance`: The instance of the `twi` driver
    * `nrfx_twi_xfer_desc_t const *p_xfer_desc`: The transfer descriptor, which includes several key elements
    * `uint32_t flags`: Flags for the transfer
  * The most important part is the `nrfx_twi_xfer_desc_t` structure, which includes:
    * `uint8_t *p_primary_buf`: The primary buffer
    * `uint16_t primary_length`: The length of the primary buffer
    * `uint8_t *p_secondary_buf`: The secondary buffer
    * `uint16_t secondary_length`: The length of the secondary buffer
    * `uint8_t address`: The address of the device for the transfer, excluding the read/write bit (7-bit address)
    * `nrfx_twi_xfer_type_t type`: The type of transfer
  * The transfer types and their differences are as follows:
    * `NRFX_TWI_XFER_TX`: After sending the device address, send data from the primary buffer. In the case of writing data to the AT24C02 chip, the memory address can be written in `p_buffer[0]`, and the data can be written from `p_buffer[1]` onwards.
    * `NRFX_TWI_XFER_TXTX`: After sending the device address, send data from the primary buffer, then send the device address again and send data from the secondary buffer.
    * `NRFX_TWI_XFER_RX`: After sending the device address, receive data into the secondary buffer.
    * `NRFX_TWI_XFER_TXRX`: After sending the device address, send data from the primary buffer, then send the device address again and receive data into the secondary buffer. In the case of reading data from the AT24C02 chip, the memory address can be written in `p_buffer[0]`, and the data can be received into `s_buffer`.
    
* Read and write to the AT24C02 chip
    ```c
    uint8_t eeprom_write_bytes(uint8_t addr, uint8_t data);
    uint8_t eeprom_write_array(uint8_t addr, uint8_t *data, uint8_t len);
    uint8_t eeprom_read_bytes(uint8_t addr);
    uint8_t eeprom_read_array(uint8_t addr, uint8_t *data, uint8_t len);
    ```
    These are functions for random read/write and sequential read/write operations on the AT24C02 chip. The specific implementation can be found in the code.

* Finally, compile and flash the program. After flashing, you can observe the success of the data read/write through the `log` or use a logic analyzer to observe the I2C communication process.