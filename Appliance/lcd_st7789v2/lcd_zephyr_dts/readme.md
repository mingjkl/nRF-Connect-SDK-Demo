## 简介
在这个demo中，我们直接在设备树中`spi0`的节点，增加`st7789v`的设备节点；然后将`zephyr,display`指定到`st7789v`的设备节点；通过LVGL图像库，实现在`st7789v`上显示图像。

## 关键代码
* 在`prj.conf`中添加关键的配置
    ```c
        CONFIG_LV_Z_MEM_POOL_NUMBER_BLOCKS=8
        CONFIG_MAIN_STACK_SIZE=2048

        CONFIG_DISPLAY=y

        CONFIG_LOG=y

        CONFIG_LVGL=y
        CONFIG_LV_MEM_CUSTOM=y
        CONFIG_LV_USE_LOG=y
        CONFIG_LV_USE_LABEL=y
        CONFIG_LV_USE_BTN=y
        CONFIG_LV_USE_IMG=y
        CONFIG_LV_USE_MONKEY=y
        CONFIG_LV_FONT_DEFAULT_MONTSERRAT_32=y
    ```
    这里主要是开启了`lvgl`图形库，以及`display`显示设备。修改了`main`线程的栈大小。

* 新建`.overlay`,并且添加`st7789v`的设备信息
    ```c
        &spi0 {
        st7789v@0 {
            compatible = "sitronix,st7789v";
            reg = <0x0>;
            cmd-data-gpios = <&gpio0 31 GPIO_ACTIVE_LOW>;
            // supply-gpios = <&gpio0 29 0>;
            label = "ST7789V";
            status = "okay";
            spi-max-frequency = <2000000>;
            width = <240>;
            height = <280>;
            x-offset = <0>;
            y-offset = <20>;
            vcom = <0x32>;
            gctrl = <0x35>;
            vrhs = <0x15>;
            mdac = <0x00>;
            gamma = <0x01>;
            colmod = <0x55>;
            lcm = <0x2c>;
            porch-param = [0c 0c 00 33 33];
            cmd2en-param = [5a 69 02 01];
            pwctrl1-param = [a4 a1];
            pvgam-param = [d0 08 0e 09 09 05 31 33 48 17 14 15 31 34];
            nvgam-param = [d0 08 0e 09 09 15 31 33 48 17 14 15 31 34];
            ram-param = [00 f8];
            rgb-param = [cd 08 14];
            vdvs = <0x20>;
            reset-gpios = <&gpio0 28 GPIO_ACTIVE_LOW>;

            supply-gpios = <&gpio0 29 GPIO_ACTIVE_LOW>;
        };

        status = "okay";
        cs-gpios = <&gpio0 30 GPIO_ACTIVE_LOW>;
    };


    &spi0_default {
        group1 {
            psels = <NRF_PSEL(SPIM_SCK, 0, 3)>, <NRF_PSEL(SPIM_MOSI, 0, 4)>;
        };
    };

    / {
        chosen {
            zephyr,display = &{/soc/spi@40003000/st7789v@0/};
        };
    };
    ```
    这里我们在`spi0`的节点下，增加了`st7789v`的设备节点。关于`st7789v`的设备节点的配置，我们可以参考sdk文档下的`zephyr\boards\shields\st7789v_generic\doc\index.rst`和lcd屏幕厂商提供的开发文档。这里面包含了`st7789v`的寄存器配置、控制IO的配置和spi的配置。

    并且在`chosen`节点下，指定了`zephyr,display`的设备节点。这样`lvgl`图形库就可以通过`zephyr,display`来找到`st7789v`的设备节点。

* 在`main.c`中，添加必要的头文件
    ```c
        #include <zephyr/device.h>
        #include <zephyr/devicetree.h>
        #include <zephyr/drivers/display.h>
        #include <zephyr/drivers/gpio.h>
        #include <lvgl.h>
        #include <stdio.h>
        #include <string.h>
        #include <zephyr/kernel.h>
        #include <hal/nrf_gpio.h>
    ```
    这里包含了`display`的头文件，`lvgl`图形库的头文件；我们需要通过gpio控制blacklight的开关，所以也包含了`gpio`的头文件。

* 获取`display`的设备节点
    ```c
        const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

        if (device_is_ready(display_dev)) {
		printf("Display device is ready\n");
        } else {
            printf("Display device is not ready\n");
        }
    ```
    成功获取到`display`的设备节点后，我们的初始化工作就完成了。

* 激活显示器
    ```c
       display_blanking_off(display_dev);
    ```
    函数`display_blanking_off`用于激活显示器。它会使显示设备恢复显示帧缓冲区的内容，如果支持的话，还会恢复背光设置。不过我没找到`st7789v`的背光使能应该怎么配置，所以单独用了一个gpio来控制背光。
    ```c
        nrf_gpio_cfg_output(LCD_BLK_PIN);
	    nrf_gpio_pin_set(LCD_BLK_PIN);
    ```

* 使用`lvgl`图形库

    这里我们创建了一个`label`对象，然后在一个死循环中，不断更新`label`的显示内容。这样就可以在`st7789v`上显示一个递增的数字。
    * 创建一个`label`对象
        ```c
            lv_obj_t *label;
            label = lv_label_create(lv_scr_act());
            
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        ```
        这里我们创建了一个`label`对象，然后将其居中对齐。

    * 设置`label`的显示内容
        ```c
            char display_text[8];
            uint16_t counter = 0;

            sprintf(display_text, "%d", counter++);
            lv_label_set_text(label, display_text);
        ```

    * 在一个死循环中，不断刷新lvgl的显示内容
        ```c
            while(1)
            {
                // ...
                lv_task_handler();
            }
        ```

* 编译并烧录程序
            
    ![image](img/git.gif)