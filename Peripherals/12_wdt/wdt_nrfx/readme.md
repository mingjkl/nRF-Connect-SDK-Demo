## 简介
这是一个`wdt`的demo，示例中设置了`5s`的超时时间，如果在`5s`内没有进行喂狗操作，会进入到超时处理函数中，并且重启设备。在这个demo中，中断函数中，实现了一个`DEBUG_PIN`输出了10个方波，用于观察`wdt`中断函数的执行。在重启之前，留给中断处理函数的时间有限，具体值是多少，大家看一下规格书是怎么描述的。

## 关键代码
* 在`prj.conf`中添加`wdt`的kconfig配置
    ```
    CONFIG_NRFX_WDT0=y
    ```
* 在`main.c`中添加必要的头文件
    ```c
    #include <nrfx_wdt.h>
    ```
* 定义一个`wdt`的实例
    ```c
    static nrfx_wdt_t wdt = NRFX_WDT_INSTANCE(0);
    ```
* 定义`wdt`的初始化结构体
    ```c
    nrfx_wdt_config_t config = NRFX_WDT_DEFAULT_CONFIG;
	config.reload_value = 5000;
    ```
    这里的`reload_value`就是超时时间，单位是ms
* 初始化`wdt`
    ```c
    nrfx_wdt_init(&wdt, &config, wdt_handler);
    ```
* 补充`wdt`的中断处理函数
    ```c
    static void wdt_handler(void)
    {
        for(uint8_t i = 0; i < 10; i++)
        {
            nrf_gpio_pin_set(DEBUG_PIN);
            k_busy_wait(1);
            nrf_gpio_pin_clear(DEBUG_PIN);
            k_busy_wait(1);
        }
    }
    ```
* 完成`wdt`的中断引入
    ```c
    IRQ_DIRECT_CONNECT(WDT_IRQn, IRQ_PRIO_LOWEST, NRFX_WDT_INST_HANDLER_GET(0), 0);
    ```
* 分配`wdt`通道
    ```c
    nrfx_wdt_channel_alloc(&wdt, &wdt_channel_id);
    ```
* 开启`wdt`
    ```c
    nrfx_wdt_enable(&wdt);
    ```
* 编译并烧录程序到开发板上，通过逻辑分析仪观察`DEBUG_PIN`的输出；这里我们没有进行喂狗，我们可以看到设备在运行了5s之后，进入到了中断处理函数中，在`DEBUG_PIN`上输出了10个方波信号，然后重启了设备。不断的往复循环。

* 喂狗操作
    ```c
    nrfx_wdt_channel_feed(&wdt, wdt_channel_id);
    ```
    我这里将喂狗动作加入到了`main`函数的死循环中，会在每秒钟喂一次狗。这样就不会进入到中断处理函数中，也就不会重启设备了。

* 我们重新编译并烧录程序到开发板上，通过逻辑分析仪观察`DEBUG_PIN`的输出；这里我们进行了喂狗，我们可以看到设备一直在运行，没有进入到中断处理函数中，也没有重启设备。
