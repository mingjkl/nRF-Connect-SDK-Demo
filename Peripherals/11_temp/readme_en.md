Here's the translation of the provided text into English:

## Introduction
This demo is about the use of the on-chip temperature sensor, with an accuracy of 0.25 degrees Celsius.

## Key Code
* Add the following to `prj.conf` to enable the on-chip temperature sensor integration:
    ```
    CONFIG_NRFX_TEMP=y
    ```
* Include the on-chip header file in `main.c`:
    ```
    #include <nrfx_temp.h>
    ```
* Define a config structure and use the default configuration:
    ```
    nrfx_temp_config_t config = NRFX_TEMP_DEFAULT_CONFIG;
    ```
* Initialize the on-chip temperature sensor:
    ```
    nrfx_temp_init(&config, temp_handler);
    ```
* Implement the interrupt function:
    ```
    static void temp_handler(int32_t temperature)
    {
        int32_t temp_celsius = nrfx_temp_calculate(temperature);
        printk("Temperature: %d\n", temp_celsius);
        
    }
    ```
    This uses the `nrfx_temp_calculate()` function to convert the sensor value to Celsius, with the unit being 0.01 degrees Celsius. Note that the sensor resolution is 0.25 degrees Celsius, so the data variation logged is in increments of 0.25 degrees Celsius.
* We also need to add an interrupt:
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TEMP), IRQ_PRIO_LOWEST, nrfx_temp_irq_handler, 0);
    ```
* Then, in the `main` function, we use a periodic polling method to trigger the temperature sensor reading:
    ```
    //....
    while (1) {
        nrfx_temp_measure();
		k_sleep(K_MSEC(100));
    }
    ```
* Compile and burn the program to the IC, and we can see the changes in the log data.