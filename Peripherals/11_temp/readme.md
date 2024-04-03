## 简介
这个demo是关于片上温度传感器的使用，精度为0.25摄氏度；

## 关键代码
* 在`prj.conf`中加入打开片上温度传感器的集成
    ```
    CONFIG_NRFX_TEMP=y

    ```
* 在`main.c`中加入片上头文件
    ```
    #include <nrfx_temp.h>
    ```
* 定义一个`config`的结构体并使用默认配置
    ```
    nrfx_temp_config_t config = NRFX_TEMP_DEFAULT_CONFIG;
    ```
* 初始化片上温度传感器
    ```
    nrfx_temp_init(&config, temp_handler);
    ```
* 补充中断函数的实现
    ```
    static void temp_handler(int32_t temperature)
    {
        int32_t temp_celsius = nrfx_temp_calculate(temperature);
        printk("Temperature: %d\n", temp_celsius);
        
    }
    ```
    这里面使用了`nrfx_temp_calculate()`函数，这个函数是将传感器的值转换为摄氏度的值，转换后的值单位是0.01摄氏度，不过注意传感器分辨率是0.25摄氏度，所以外面可以看到log处理的数据变化量是0.25摄氏度；
* 我们还需要补充中断引入
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TEMP), IRQ_PRIO_LOWEST, nrfx_temp_irq_handler, 0);
    ```
* 然后我们在`main`函数中使用定期轮询的方式触发温度传感器的读取
    ```
    //....
    while (1) {
        nrfx_temp_measure();
		k_sleep(K_MSEC(100));
    }
    ```
* 编译并烧录程序到ic上，我们可以看到log数据的变化；