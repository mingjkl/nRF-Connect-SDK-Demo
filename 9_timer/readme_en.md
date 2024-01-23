## Introduction
This demo demonstrates the use of the timer peripheral to generate timed interrupts. In this demo, a timer with a 5-second interval is configured. When the timer interrupt occurs, it logs a message. It's important to note that this demo is based on SDK version v2.4.2. When using other SDK versions, there may be some differences. If you encounter compilation issues, please make necessary modifications based on your specific SDK version.

## Key Code
* Configure the timer peripheral in prj.conf:
    ```c
    CONFIG_NRFX_TIMER0=y
    CONFIG_LOG=y
    ```
    Here, the log module is included. We use the log module to print a message when the interrupt occurs. However, we won't go into detail about the log part. You can refer to the logging demo to learn how to use the log module.

* Include the header file in main.c:
    ```c
    #include <nrfx_timer.h>
    ```

* Define an instance of the timer peripheral:
    ```c
    nrfx_timer_t timer = NRFX_TIMER_INSTANCE(0);
    ```

* Complete the configuration of the timer peripheral:
    ```c
    nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG;
	
    config.frequency = NRF_TIMER_FREQ_16MHz;
    config.bit_width = NRF_TIMER_BIT_WIDTH_32;
    nrfx_timer_init(&timer, &config, timer_handler);
    ```
    In this section, we use the default configuration and set the timer's frequency to 16MHz, and the bit width to 32 bits. Finally, we assign the timer's interrupt handling function as `timer_handler`.

* Define the `timer_handler` function:
    ```c
    static void timer_handler(nrf_timer_event_t event_type, void * p_context)
    {
        if (event_type == NRF_TIMER_EVENT_COMPARE0)
        {
            LOG_INF("Timer event compare 0");
        }
    }
    ```
    In this function, we only handle the `NRF_TIMER_EVENT_COMPARE0` event.

* Set up the interrupt connection:
    ```c
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_TIMER_INST_HANDLER_GET(0), 0);
    ```

* Configure the timer peripheral's compare value:
    ```c
    nrfx_timer_clear(&timer);

    uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer, 5000);

    nrfx_timer_extended_compare(&timer, NRF_TIMER_CC_CHANNEL0, desired_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    ```
    First, we clear the timer peripheral, then calculate the `desired_ticks` value for a 5-second interval. Finally, we configure the timer peripheral's compare register with the `desired_ticks` value. When the timer peripheral's counter reaches the value in the compare register, an interrupt is triggered. Additionally, we configure the timer peripheral to automatically clear its counter when the compare value is reached (NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK).

* Enable the timer peripheral:
    ```c
    nrfx_timer_enable(&timer);
    ```

* Compile and flash the program. You can see in the log output from the RTT Viewer that the timer interrupt triggers every 5 seconds and logs a message:
    ```
    00> *** Booting Zephyr OS build v3.3.99-ncs1 ***
    00> [00:00:05.379,730] <inf> timer: Timer event compare 0
    00> [00:00:10.382,080] <inf> timer: Timer event compare 0
    00> [00:00:15.385,803] <inf> timer: Timer event compare 0
    00> [00:00:20.389,862] <inf> timer: Timer event compare 0
    00> [00:00:25.394,531] <inf> timer: Timer event compare 0
    00> [00:00:30.399,047] <inf> timer: Timer event compare 0
    00> [00:00:35.404,846] <inf> timer: Timer event compare 0
    ```