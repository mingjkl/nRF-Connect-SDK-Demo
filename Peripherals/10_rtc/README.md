## 简介
这个demo我们演示的是关于rtc的使用，在demo中，我们为rtc设定了一个30s的compare；每次compare触发中断，会通过log打印当前rtc的时间，并且将rtc清空；

## 关键代码
* 在prj_conf.h中，完成对rtc的配置
    ```
    CONFIG_NRFX_RTC0=y
    ```

* 加入rtc的头文件
    ```
    #include <nrfx_rtc.h>
    ```

* 定义一个rtc的实例
    ```
    static nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(0);
    ```

* 定义一个rtc配置的结构体
    ```
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    ```

* 初始化rtc
    ```
    nrfx_rtc_init(&rtc, &config, rtc_handler);
    ```

* 实现中断函数
    ```
    static void rtc_handler(nrfx_rtc_int_type_t int_type)
    {
        if(int_type == NRFX_RTC_INT_COMPARE0)
        {
            unsigned int ticks =  ticks_to_us(nrfx_rtc_counter_get(&rtc), prescaler_to_freq(rtc.p_reg->PRESCALER));
            
            if(ticks > 1000)
                LOG_INF("RTC: %d ms\n", ticks / 1000);
            else
                LOG_INF("RTC: %d us\n", ticks);

            nrfx_rtc_counter_clear(&rtc);
        }
    }
    ```
    中断函数中，我们通过nrfx_rtc_counter_get获取当前rtc的时间，并打印出来，通过nrfx_rtc_counter_clear清空rtc的时间；

* 中断引入
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_RTC_INST_HANDLER_GET(0), 0);
    ```

* 配置rtc的compare
    ```
    nrfx_rtc_cc_set(&rtc, 0, overflow_ms_to_ticks(30000, freq), true);
    ```
    这里我们设置了一个30s的compare，当rtc计数到30s时，会触发中断；

* 启动rtc
    ```
    nrfx_rtc_enable(&rtc);
    ```

* 编译并烧录程序，观察log，我们可以看到，每隔30s，rtc会触发中断，打印出当前rtc的时间；