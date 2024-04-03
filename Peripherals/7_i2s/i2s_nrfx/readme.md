## 简介
这是一个`nrfx_i2s`的演示使用程序；demo只演示了通过`i2s`接口输出`44.1KHz`的`16bit`立体声音频信号；demo中只演示了数据输出，数据输入的部分没有演示；

## 关键代码
* 配置Kconfig
    ```c
    CONFIG_LOG=y
    CONFIG_NRFX_I2S=y
    ```
* 加入头文件
    ```c
    #include <zephyr/logging/log.h>
    #include <nrfx_i2s.h>
    #include <hal/nrf_gpio.h>
    ```
* 定义`i2s`配置
    ```c
    #define I2S_SCK_PIN NRF_GPIO_PIN_MAP(0, 31)
    #define I2S_LRCK_PIN NRF_GPIO_PIN_MAP(0, 30)
    #define I2S_MCK_PIN NRF_GPIO_PIN_MAP(0, 29)
    #define I2S_SDOUT_PIN NRF_GPIO_PIN_MAP(0, 28)
    #define I2S_SDIN_PIN NRF_GPIO_PIN_MAP(0, 4)

    // ...

    nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(I2S_SCK_PIN,
													   I2S_LRCK_PIN,
													   I2S_MCK_PIN,
													   I2S_SDOUT_PIN,
													   I2S_SDIN_PIN);
	config.channels = NRF_I2S_CHANNELS_STEREO;
	config.sample_width = NRF_I2S_SWIDTH_16BIT;
	config.alignment = NRF_I2S_ALIGN_LEFT;
	config.mode = NRF_I2S_MODE_MASTER;
	config.ratio = NRF_I2S_RATIO_32X;
	config.mck_setup = NRF_I2S_MCK_32MDIV23;

	
	nrfx_i2s_init(&config, i2s_handler);
    ```
* 引入中断
  ```
  IRQ_DIRECT_CONNECT(I2S_IRQn, IRQ_PRIO_LOWEST, nrfx_i2s_irq_handler, 0);
  ```
* 补充`i2s`中断处理函数
  ```c
    static void i2s_handler(nrfx_i2s_buffers_t const * p_released, uint32_t status)
    {
        if(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)
        {
            i2s_buf_sw = i2s_buf_sw ? 0 : 1;

            nrfx_i2s_buffers_t next_buffers = {
                .p_tx_buffer = m_buffer_tx[i2s_buf_sw],
                .p_rx_buffer = m_buffer_rx[i2s_buf_sw],
            };
            nrfx_i2s_next_buffers_set(&next_buffers);
        }

        if(status & NRFX_I2S_STATUS_TRANSFER_STOPPED)
        {
            nrfx_i2s_buffers_t init_buffers = {
            .p_tx_buffer = m_buffer_tx[0],
            .p_rx_buffer = m_buffer_rx[0],
        };
            nrfx_i2s_start(&init_buffers, I2S_DATA_BLOCK_WORDS, 0);
        }
    }
  ```
  这里我们使用了`double buffer`的方式，当`i2s`需要下一个缓冲区时，我们切换缓冲区；当`i2s`传输停止时，我们重新启动`i2s`传输；

* 启动`i2s`传输
    ```c
        nrfx_i2s_buffers_t init_buffers = {
            .p_tx_buffer = m_buffer_tx[0],
            .p_rx_buffer = m_buffer_rx[0],
        };

        nrfx_i2s_start(&init_buffers, I2S_DATA_BLOCK_WORDS, 0);
    ```

* 编译并烧录程序，我们可以通过逻辑分析仪观察到`i2s`输出的信号；我们在demo中往buffer中填充了`0x11223344`,我们可以看看有没有写入到`i2s`输出的信号中；
* 
    ![write](img/tx.PNG)