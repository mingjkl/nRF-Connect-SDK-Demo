
## Introduction
这个示例是演示使用NRF5 SDK实现的ADC数据采集，然后将采集的ADC值转换成电压值并LOG输出到RTT上。

## Steps
* 创建一个新的Hello world工程
* 在prj.conf中添加以下配置
```
    CONFIG_LOG=y
    CONFIG_RTT_CONSOLE=y
    CONFIG_USE_SEGGER_RTT=y

    CONFIG_NRFX_SAADC=y
```
* 在main.c中添加必要的头文件
```
    #include "zephyr/logging/log.h"
    #include "nrfx_saadc.h"
    #include <zephyr/kernel.h>

```
* 添加adc初始化函数
```
int adc_init(void)
{
	nrfx_err_t err;
	err = nrfx_saadc_init(NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY);
	err = nrfx_saadc_channel_config(&single_channel);
	uint32_t channels_mask = nrfx_saadc_channels_configured_get();

	LOG_INF("ADC channels mask: %d\n", channels_mask);
	
	err = nrfx_saadc_simple_mode_set(channels_mask, NRF_SAADC_RESOLUTION_12BIT, NRF_SAADC_OVERSAMPLE_DISABLED, NULL);
	err = nrfx_saadc_buffer_set(buffer, CHANNEL_COUNT);
}
```
* 其中nrfx_saadc_init为saadc的驱动初始化，输入参数为中断优先级，这里使用默认的优先级。
* nrfx_saadc_channel_config完成通道的配置，我们定义一个关于通道配置的结构体
```
    static nrfx_saadc_channel_t single_channel = NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN3, 0);
```
这个配置为单端模式，输入通道为AIN3，增益为1/6，参考电压为0.6V，输入电压范围为0~3.6V。

* nrfx_saadc_channels_configured_get();记录了当前配置的通道配置情况。
* nrfx_saadc_simple_mode_set(channels_mask, NRF_SAADC_RESOLUTION_12BIT, NRF_SAADC_OVERSAMPLE_DISABLED, NULL),设置通道分辨率为12bit，不使用过采样，事件回调。
* nrfx_saadc_buffer_set(buffer, CHANNEL_COUNT);在开始 SAADC 转换之前，您需要调用此函数来设置一个缓冲区，SAADC 将把采样数据存储到这个缓冲区中。

至此完成了adc_init函数的编写。

* 添加adc采集函数
```
    int adc_value_get(void)
    {
        nrfx_err_t err;
        
        err = nrfx_saadc_offset_calibrate(NULL);
        err = nrfx_saadc_mode_trigger();
        return buffer[0];
    }
```
  * nrfx_saadc_offset_calibrate(NULL);校准ADC的偏移量，这里不使用回调函数。
  * nrfx_saadc_mode_trigger();开始采集数据。
* 添加adc转换函数，将ADC值转换为电压值
```
    int adc_to_voltage(int adc_value)
    {
        return (adc_value * 3600) / 4096;
    }
```
* 在主函数中实现adc的初始化，采集，转换，输出
```
    
    int main(void)
    {
        LOG_INF("ADC Demo %s\n", CONFIG_BOARD);
        adc_init();
        
        while (1)
        {
            LOG_INF("vol=%d\r\n", adc_to_voltage(adc_value_get()));
            k_msleep(10);
        }
        
        return 0;
    }

```
* 编译下载运行，可以看到RTT输出了ADC采集的电压值。在代码中选用了AIN3通道，可以通过外部电压表测量AIN3的电压值，与RTT输出的值进行对比，可以看到RTT输出的值与外部电压表测量的值相同。
