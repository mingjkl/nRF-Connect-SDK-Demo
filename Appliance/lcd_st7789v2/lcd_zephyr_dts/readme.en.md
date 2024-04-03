## Introduction
In this demo, we directly add the `st7789v` device node in the device tree under `spi0` node, then specify `zephyr,display` to the `st7789v` device node, and use the LVGL graphics library to display images on the `st7789v`.

## Key Code
* Add key configurations in `prj.conf`
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
    Here, the LVGL graphics library and display device are enabled. The stack size of the `main` thread is modified.

* Create a `.overlay` file and add `st7789v` device information
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
    Here, we add the `st7789v` device node under the `spi0` node. The configuration of the `st7789v` device node can be referred to in the SDK documentation under `zephyr\boards\shields\st7789v_generic\doc\index.rst` and the development documentation provided by the LCD screen manufacturer. This includes the register configuration of `st7789v`, control IO configuration, and SPI configuration.

    Additionally, under the `chosen` node, the `zephyr,display` device node is specified. This allows the LVGL graphics library to find the `st7789v` device node through `zephyr,display`.

* In `main.c`, include necessary header files
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
    This includes the header files for `display`, the LVGL graphics library, and GPIO for controlling the backlight.

* Retrieve the `display` device node
    ```c
        const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

        if (device_is_ready(display_dev)) {
		printf("Display device is ready\n");
        } else {
            printf("Display device is not ready\n");
        }
    ```
    Once the `display` device node is successfully retrieved, our initialization work is complete.

* Activate the display
    ```c
       display_blanking_off(display_dev);
    ```
    The `display_blanking_off` function is used to activate the display. It restores the content of the display frame buffer and, if supported, the backlight setting. However, since I couldn't find how to configure the backlight enable for `st7789v`, I separately used a GPIO to control the backlight.
    ```c
        nrf_gpio_cfg_output(LCD_BLK_PIN);
	    nrf_gpio_pin_set(LCD_BLK_PIN);
    ```

* Use the LVGL graphics library

    Here, we create a `label` object and continuously update the display content of the `label` in a loop to display an incrementing number on the `st7789v`.
    * Create a `label` object
        ```c
            lv_obj_t *label;
            label = lv_label_create(lv_scr_act());
            
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        ```
        Here, we create a `label` object and align it to the center.

    * Set the display content of the `label`
        ```c
            char display_text[8];
            uint16_t counter = 0;

            sprintf(display_text, "%d", counter++);
            lv_label_set_text(label, display_text);
        ```

    * Continuously refresh the LVGL display content in a loop
        ```c
            while(1)
            {
                // ...
                lv_task_handler();
            }
        ```

* Compile and flash the program
            
    ![image](img/git.gif)
```