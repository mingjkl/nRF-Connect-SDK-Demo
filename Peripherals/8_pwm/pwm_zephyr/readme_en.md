## Introduction
This demo is implemented using `Zephyr`'s `API` to generate `pwm` signals. The demo utilizes two pins as outputs for `channel1` and `channel2` of `pwm0`.

## Key Code
* Configure `pwm` in `prj.conf`:
    ```c
    CONFIG_PWM=y
    ```
* Create a `.overlay` file and add `pwm` pin configurations:
    ```c
    &pwm0_default {
    group2 {
        psels = <NRF_PSEL(PWM_OUT1, 0, 29)>, <NRF_PSEL(PWM_OUT2, 0, 28)>;
    };
    };

    &pwm0 {
    status = "okay";
    };
    ```
    I'm using `nrf52840dk`, where `channel0` of `pwm0` is already assigned to `P0.13` for controlling an LED in the original device tree. Therefore, I left it unchanged. If you want to see the fading effect, you can also try `channel0`. I've reassigned the pins for `channel1` and `channel2` since I'm observing their output with a logic analyzer.

* Include necessary header files in `main.c`:
    ```c
    #include <zephyr/drivers/pwm.h>
    #include <zephyr/device.h>
    #include <zephyr/devicetree.h>
    ```

* Define a `pwm` device:
    ```c
    static const struct device *pwm_dev = DEVICE_DT_GET(DT_NODELABEL(pwm0));
    ```

* Check if the `pwm` device is ready:
    ```c
    if (!device_is_ready(pwm_dev)) {
		printk("PWM device is not ready\n");
		return -1;
	}
	else {
		printk("PWM device is ready\n");
	}
    ```

* Configure the frequency and duty cycle of `pwm`:
    ```c
    uint32_t freq = 1000;						   // Hz
	uint32_t pulse_width = 25;					   // percentage
	uint32_t period = 1000000000 / freq;		   // ns
	uint32_t pulse = (period * pulse_width) / 100; // ns

	ret = pwm_set(pwm_dev, 1, period, pulse, NULL);
	ret = pwm_set(pwm_dev, 2, period, pulse * 2, NULL);
    ```
    Here, I've set the `pwm` frequency to `1000Hz`, with duty cycles of `25%` and `50%` for `channel1` and `channel2`, respectively.

* Compile and flash the program, then observe the output of `channel1` and `channel2` with a logic analyzer.

![pwm0](img/img.PNG)