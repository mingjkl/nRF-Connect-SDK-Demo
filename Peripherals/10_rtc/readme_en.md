## Introduction
In this demo, we demonstrate the use of RTC (Real-Time Clock). In the demo, we set a 30s compare value for the RTC. Whenever the compare event triggers an interrupt, the current RTC time is logged, and the RTC is cleared.

## Key Code
* In `prj_conf.h`, complete the configuration for RTC:
    ```
    CONFIG_NRFX_RTC0=y
    ```

* Include the RTC header file:
    ```
    #include <nrfx_rtc.h>
    ```

* Define an instance of the RTC:
    ```
    static nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(0);
    ```

* Define a structure for RTC configuration:
    ```
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    ```

* Initialize the RTC:
    ```
    nrfx_rtc_init(&rtc, &config, rtc_handler);
    ```

* Implement the interrupt handler:
    ```
    static void rtc_handler(nrfx_rtc_int_type_t int_type)
    {
        if(int_type == NRFX_RTC_INT_COMPARE0)
        {
            unsigned int ticks =  ticks_to_us(nrfx_rtc_counter_get(&rtc), prescaler_to_freq(rtc.p_reg->PRESCALER));
            
            if(ticks > 1000)
                LOG_INF("RTC: %d ms\n", ticks / 1000);
            else
                LOG_INF("RTC: %d us\n", ticks);

            nrfx_rtc_counter_clear(&rtc);
        }
    }
    ```
    In the interrupt handler, we use `nrfx_rtc_counter_get` to retrieve the current RTC time and print it. We then clear the RTC time using `nrfx_rtc_counter_clear`.

* Connect the interrupt:
    ```
    IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_RTC_INST_HANDLER_GET(0), 0);
    ```

* Configure the RTC compare value:
    ```
    nrfx_rtc_cc_set(&rtc, 0, overflow_ms_to_ticks(30000, freq), true);
    ```
    Here, we set a 30s compare value for the RTC. When the RTC counts to 30s, it triggers an interrupt.

* Enable the RTC:
    ```
    nrfx_rtc_enable(&rtc);
    ```

* Compile and flash the program. Observe the log, and you will see that every 30s, the RTC triggers an interrupt and logs the current RTC time.