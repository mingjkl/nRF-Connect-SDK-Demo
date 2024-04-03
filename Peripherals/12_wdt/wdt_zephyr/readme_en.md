## Introduction
This is a `wdt` demo implemented using the `Zephyr API`. In this demo, a timeout period of `5s` is configured for the `wdt`. When the timeout occurs, it triggers the `wdt` timeout interrupt. Subsequently, handling is performed in the interrupt handling function, followed by a reset of the `SOC`.

## Key Code
* Configure `wdt` in `prj.conf`:
    ```c
    CONFIG_WATCHDOG=y
    ```

* Create a `.overlay` file and configure `wdt`:
    ```c
    &wdt {
    status = "okay";
    };
    ```

* Include necessary header files in `main.c`:
    ```c
    #include <zephyr/drivers/watchdog.h>
    #include <zephyr/device.h>
    #include <zephyr/devicetree.h>
    ```

* Define the `wdt` device:
    ```c
    static const struct device *wdt = DEVICE_DT_GET(DT_NODELABEL(wdt0));
    ```

* Check if `wdt` is available:
    ```c
    if (!device_is_ready(wdt)) {
		printk("Watchdog device %s is not ready\n", wdt->name);
		return 1;
	}
    ```

* Configure the timeout period of `wdt`:
    ```c
    struct wdt_timeout_cfg wdt_config = {
		.window.min = 0,		// ms
		.window.max = 5000,		// ms
		.callback = wdt_callback,
		.flags = WDT_FLAG_RESET_SOC,
	};
	wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
    ```
    Here, we configure a timeout period of `5s`. When the timeout occurs, it triggers the `wdt` timeout interrupt. Optionally, the interrupt callback function (`wdt_callback`) can be set to `NULL`. Note that the value of `.window.min` seems to be limited to `0`; otherwise, an error may occur. The `.flags` configure the behavior of `wdt`, and here we set `WDT_FLAG_RESET_SOC`. Omitting this flag may result in an error.

    The `wdt_install_timeout` function returns a `wdt_channel_id`, which is the channel number of the `wdt` and will be used for feeding the dog later. If a negative value is returned, it indicates an error code.

* Start the `wdt`:
    ```c
    wdt_setup(wdt, NULL);
    ```

* Feed the dog:
    ```c
    wdt_feed(wdt, wdt_channel_id);
    ```

* Handle in the `wdt` timeout interrupt function:
    ```c
    static void wdt_callback(const struct device *dev, int channel_id)
    {
        for(uint8_t i = 0; i < 10; i++)
        {
            nrf_gpio_pin_toggle(DEBUG_PIN);
        }
    }
    ```
    Here, for the convenience of observing whether the interrupt handling function is entered, we toggle `DEBUG_PIN` `10` times.

* Compile and flash the program. We can observe through a serial debugging tool whether the `SOC` resets when the dog is not fed. Also, we can observe through a logic analyzer whether `DEBUG_PIN` toggles `10` times after the timeout.