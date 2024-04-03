## Introduction
This is a demo for `wdt` (Watchdog Timer), where a timeout of `5s` is set. If no feeding operation is performed within `5s`, it will enter the timeout handler function and restart the device. In this demo, within the interrupt function, a `DEBUG_PIN` outputs 10 square waves for observing the execution of the `wdt` interrupt function. The time allocated to the interrupt handler before restarting is limited; for the specific value, please refer to how it's described in the specification.

## Key Code
* Add the `wdt` kconfig configuration in `prj.conf`
    ```
    CONFIG_NRFX_WDT0=y
    ```
* Add the necessary header file in `main.c`
    ```c
    #include <nrfx_wdt.h>
    ```
* Define a `wdt` instance
    ```c
    static nrfx_wdt_t wdt = NRFX_WDT_INSTANCE(0);
    ```
* Define the initialization structure for `wdt`
    ```c
    nrfx_wdt_config_t config = NRFX_WDT_DEFAULT_CONFIG;
	config.reload_value = 5000;
    ```
    Here, `reload_value` is the timeout period, in milliseconds.
* Initialize `wdt`
    ```c
    nrfx_wdt_init(&wdt, &config, wdt_handler);
    ```
* Supplement the `wdt` interrupt handler function
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
* Complete the introduction of `wdt` interrupt
    ```c
    IRQ_DIRECT_CONNECT(WDT_IRQn, IRQ_PRIO_LOWEST, NRFX_WDT_INST_HANDLER_GET(0), 0);
    ```
* Allocate `wdt` channel
    ```c
    nrfx_wdt_channel_alloc(&wdt, &wdt_channel_id);
    ```
* Enable `wdt`
    ```c
    nrfx_wdt_enable(&wdt);
    ```
* Compile and flash the program to the development board, observe the `DEBUG_PIN` output through a logic analyzer; here we did not feed the dog, and we can see that after running for 5s, the device entered the interrupt handler function, output 10 square wave signals on `DEBUG_PIN`, and then restarted the device. This cycle repeats continuously.

* Feeding the dog operation
    ```c
    nrfx_wdt_channel_feed(&wdt, wdt_channel_id);
    ```
    I have added the feeding operation into the infinite loop of the `main` function, feeding the dog every second. This way, it will not enter the interrupt handler function, nor will the device restart.

* We recompile and flash the program to the development board, observe the `DEBUG_PIN` output through a logic analyzer; here, by feeding the dog, we can see the device continuously running without entering the interrupt handler function or restarting the device.