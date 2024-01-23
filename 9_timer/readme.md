## 简介
这个demo是演示timer外设的使用，使用timer外设来产生定时中断；在demo中配置了一个5s的定时器，当定时器中断发生时，会log一条信息。这里注意，demo使用的sdk版本为v2.4.2，使用其他版本的sdk时可能会有部分出入，如遇到编译不过的问题，请根据实际情况进行修改。

## 关键代码
* 在prj.conf中配置timer外设
    ```
    CONFIG_NRFX_TIMER0=y
    CONFIG_LOG=y
    ```
    这里面我们把log模块也加进来了，在中断触发的时候我们用log模块来打印一条信息。不过后面设计道log部分我们不重复赘述了，大家可以看logging的demo了解log模块是如何使用的。

* 在main.c中加入头文件
    ```
    #include <nrfx_timer.h>
    ```

* 定义timer外设的实例
    ```
    nrfx_timer_t timer = NRFX_TIMER_INSTANCE(0);
    ```

* 完成timer外设的配置
    ```
    nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG;
	
	config.frequency = NRF_TIMER_FREQ_16MHz;
	config.bit_width = NRF_TIMER_BIT_WIDTH_32;
    nrfx_timer_init(&timer, &config, timer_handler);
    ```
    这里面，我们使用了默认的配置，然后把timer的频率设置为16MHz，bit_width设置为32位，最后把timer的中断处理函数设置为timer_handler。

* 我们补充一下timer_handler函数的定义
    ```
    static void timer_handler(nrf_timer_event_t event_type, void * p_context)
    {
        if(event_type == NRF_TIMER_EVENT_COMPARE0)
        {
            LOG_INF("Timer event compare 0");
        }
    }
    ```
    这里面，我们只处理了NRF_TIMER_EVENT_COMPARE0事件；

* 进行中断引入
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_TIMER_INST_HANDLER_GET(0), 0);
    ```

* 配置timer外设的比较值
    ```
    nrfx_timer_clear(&timer);

	uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer, 5000);

	nrfx_timer_extended_compare(&timer, NRF_TIMER_CC_CHANNEL0, desired_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    ```
    我们先将timer外设清零，然后计算出5s对应的ticks值，最后将ticks值配置到timer外设的比较寄存器中，当timer外设的计数值达到比较寄存器的值时，就会触发中断。同时还配置了timer外设的比较寄存器达到比较值时，自动清零timer外设的计数值(NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK)。

* 使能timer外设
    ```
    nrfx_timer_enable(&timer);
    ```

* 编译并烧录程序，我们通过RTT Viewer输出的log可以看到，每隔5s时间timer中断触发一次并输出一条信息；
    ```
    00> *** Booting Zephyr OS build v3.3.99-ncs1 ***
    00> [00:00:05.379,730] <inf> timer: Timer event compare 0
    00> [00:00:10.382,080] <inf> timer: Timer event compare 0
    00> [00:00:15.385,803] <inf> timer: Timer event compare 0
    00> [00:00:20.389,862] <inf> timer: Timer event compare 0
    00> [00:00:25.394,531] <inf> timer: Timer event compare 0
    00> [00:00:30.399,047] <inf> timer: Timer event compare 0
    00> [00:00:35.404,846] <inf> timer: Timer event compare 0
    ```