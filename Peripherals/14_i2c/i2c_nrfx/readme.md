## 简介
这是一个使用`nrfx_twi`驱动的`I2C`的demo，这个demo通过读写`AT24C02`芯片来验收`nrfx_twi`的使用。

## 关键代码
* 在`prj.conf`中添加`twi`的配置
    ```
    CONFIG_NRFX_TWI0=y
    CONFIG_LOG=y
    ```
    这里我们使用的是`twi0`,同时我们也打开了`log`的配置，用于打印调试信息，不过后面关于`log`的内容我们就不介绍了。
* 在`main.c`中加入必要的头文件
    ```c
    #include <nrfx_twi.h>
    #include <hal/nrf_gpio.h>
    #include <zephyr/logging/log.h>
    ```
* 加入`i2c`引脚的宏定义
    ```c
    #define SDA_PIN NRF_GPIO_PIN_MAP(0, 31)
    #define SCL_PIN NRF_GPIO_PIN_MAP(0, 30)
    ```
* 定义一个`twi`的实例
    ```c
    static const nrfx_twi_t twi = NRFX_TWI_INSTANCE(0);
    ```
* 定义`twi`的配置
    ```c
    nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG(SCL_PIN, SDA_PIN);
    ```
    我们这里使用默认配置；
* 初始化`twi`
    ```c
    nrfx_twi_init(&twi, &config, twi_handler, NULL);
    ```
* 补充`twi`的中断处理函数
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
* 中断引入
    ```c
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TWI_INST_GET(0)), IRQ_PRIO_LOWEST,
                       NRFX_TWI_INST_HANDLER_GET(0), 0);
    ```
* 使能`twi`
    ```c
    nrfx_twi_enable(&twi);
    ```
* `i2c`的数据交互
  * 数据交互主要使用`nrfx_twi_xfer`函数实现，包含三个参数：
    * `rfx_twi_t const *p_instance`, `twi`的实例
    * `nrfx_twi_xfer_desc_t const *p_xfer_desc`, 交互的描述, 这里面几个关键的内容
    * `uint32_t flags`, 交互的标志
  * 这里面最关键的是`nrfx_twi_xfer_desc_t`的内容；
    * `uint8_t *p_primary_buf`, 主缓冲区
    * `uint16_t primary_length`, 主缓冲区的长度
    * `uint8_t *p_secondary_buf`, 从缓冲区
    * `uint16_t secondary_length`, 从缓冲区的长度
    * `uint8_t address`, 交互的地址，设备地址，不包含读写位的7位地址
    * `nrfx_twi_xfer_type_t type`, 交互的类型
  * 交互类型`type`和区别:
    * `NRFX_TWI_XFER_TX`,发送完设备地址后，发送主缓冲区的数据，在`at24c02`的写数据过程中，可以将内存地址写在`p_buffer[0]`中，然后将数据写在`p_buffer[1]`及以后的位置
    * `NRFX_TWI_XFER_TXTX`,发送完设备地址后，发送主缓冲区的数据，然后再发送一次地址，发送从缓冲区的数据；
    * `NRFX_TWI_XFER_RX`,发送完设备地址后，接收从缓冲区的数据
    * `NRFX_TWI_XFER_TXRX`,发送完设备地址后，发送主缓冲区的数据，然后再发送一次地址，接收从缓冲区的数据；在`at24c02`的读数据过程中，可以将内存地址写在`p_buffer[0]`中，然后使用`s_buffer`来接收数据
    
* 读写`AT24C02`
    ```c
    uint8_t eeprom_write_bytes(uint8_t addr, uint8_t date);
    uint8_t eeprom_write_array(uint8_t addr, uint8_t *data, uint8_t len);
    uint8_t eeprom_read_bytes(uint8_t addr);
    uint8_t eeprom_read_array(uint8_t addr, uint8_t *data, uint8_t len);
    ```
    这里是关于`AT24C02`的随机读写和连续读写的函数，具体的实现可以参考代码。

* 然后就是程序的编译和烧录，烧录完成后，我们可以通过`log`观察数据是否读写成功，也可以通过逻辑分析仪观察`i2c`的交互过程。