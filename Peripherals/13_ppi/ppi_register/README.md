## 简介
这是一个基于`uart_nrfx`的demo写的ppi demo，但是这里面的`ppi`功能是使用寄存器配置实现的；是的，我使用`nrfx_ppi`库没实现出来，但是文档我就是不看！

好吧，回到这个demo，这个demo是将`uart`的`Data sent from TXD `事件和一个`gpio`连接通过`ppi`连接起来，当事件触发，就会自动触发`gpio`的任务，并上拉引脚；这个demo本身没有事件的意义，只是为了演示`ppi`的使用，实现硬件和硬件之间的连接，而不经过`cpu`的干预；

 ![pwm1](image/1.jpg)

## 关键代码
* 我们复制一个`3_uart\uart_nrfx`的工程
* 
* 其实不影响，但是我们还是改一下`prj.conf`的log console输出
  ```
    CONFIG_USE_SEGGER_RTT=y
    CONFIG_RTT_CONSOLE=y
    CONFIG_UART_CONSOLE=n
  ```
  我们将log的输出改为通过`rtt`输出，而不是`uart`输出；

* 引脚配置
  ```
    #define UART_TX_PIN NRF_GPIO_PIN_MAP(0, 31)
    #define UART_RX_PIN NRF_GPIO_PIN_MAP(0, 8)

    #define DEBUG_PIN NRF_GPIO_PIN_MAP(0,30)
  ```
  `uart`的`tx`引脚在`nRF52840dk`上用于`debug`的`uart`是`P0.06`，我们将`uart`的`tx`引脚改为`P0.31`，这里的变更，只是方便我们使用逻辑分析仪连接`P0.31`方便我们观察；同时定义一个`gpio`的引脚`P0.30`，用于连接`ppi`的`task`，这个引脚我们用于观察`ppi`的触发情况；

* 添加头文件到`main.c`
  ```
    #include <nrfx_ppi.h>
    #include <nrfx_gpiote.h>
  ```

* 配置`gpiote`
  ```
  NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) |
							(DEBUG_PIN << GPIOTE_CONFIG_PSEL_Pos) |
							(GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_POLARITY_Pos);
  ```
  这里我们配置`gpiote`的`task`，`task`模式，`task`的引脚为`DEBUG_PIN`，`task`的触发方式为`GPIOTE_CONFIG_OUTINIT_High`，也就是说，当`task`触发时，`DEBUG_PIN`会被拉高；

* 获取分配的`ppi`通道
  ```
  nrf_ppi_channel_t ppi_channel;
  nrfx_ppi_channel_alloc(&ppi_channel);
  ```
  这里我们获取一个`ppi`通道，这个通道是由`nrfx_ppi_channel_alloc`分配的，这个函数会返回一个`ppi`通道，我们将这个通道保存在`ppi_channel`中；

* 配置`ppi`通道
  ```
  NRF_PPI->CH[ppi_channel].EEP = (uint32_t)&NRF_UART0->EVENTS_TXDRDY;
  NRF_PPI->CH[ppi_channel].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];
  ```
  这里我们配置`ppi`通道，`EEP`为`uart`的`Data sent from TXD`事件，`TEP`为刚刚我们配置的`gpiote`的`task`；

* 然后当然是使能`ppi`通道
  ```
  NRF_PPI->CHENSET = 1 << ppi_channel;
  ```
  到这里，我们已经实现了我们前面说的功能了，当`uart`的`Data sent from TXD`事件触发时，会触发`gpiote`的`task`，`task`会将`DEBUG_PIN`拉高；那么拉高了，我们是不是在适当的时候将`DEBUG_PIN`拉低，不然只能看到`DEBUG_PIN`变化后一直是高电平了；

* 加入`gpiote`的`task`复位
  我们加在串口的中断函数中，在`NRFX_UARTE_EVT_TX_DONE`的事件中加入
    ```
    static void uart_handler(nrfx_uarte_event_t const * p_event, void * p_context)
    {

        switch (p_event->type) {
            case NRFX_UARTE_EVT_TX_DONE:
                NRF_GPIOTE->TASKS_CLR[0] = PPI_TASKS_CHG_EN_EN_Msk | ppi_channel;
                break;
            // .....
            default:
                break;
        }

    }
    ```

* 编译并烧录程序，我们使用示波器或者逻辑分析仪观察`UART_TX_PIN`和`DEBUG_PIN`的变化
  
  ![pwm1](image/2.png)
  
  我们使用串口调试工具向开发板发送数据，我发送的是`123\r\n`;因为`3_uart\uart_nrfx`的工程实现的功能是串口数据的回环，所以开发板收到了我们发送的数据，会将我们发送的数据重新发送回来；这时候我们可以看到`UART_TX_PIN`和`DEBUG_PIN`的变化；

  我们通过示波器或者逻辑分析仪可以看到，`UART_TX_PIN`完成第一个byte的停止位发送后，`DEBUG_PIN`从低电平变为高电平；完整的数据字段`123\r\n`发送完成后，`DEBUG_PIN`又变为低电平；