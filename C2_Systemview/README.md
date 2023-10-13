## Introduction
This is an example that demonstrates how to use SystemView in Zephyr.

## Steps
* Create a "hello world" demo.

* Add Kconfig configuration in prj.conf:
    ```
    CONFIG_RTT_CONSOLE=y
    CONFIG_SEGGER_SYSTEMVIEW=y
    CONFIG_SEGGER_SYSVIEW_RTT_BUFFER_SIZE=102400
    CONFIG_TRACING=y
    CONFIG_LOG=y
    ```

* Create two threads in main.c for testing.

* Compile and flash the program.

* Open SystemView:
  * Click "Start Recording".
  * Select J-Link as the recorder.
  * J-Link Connection: USB.
  * Target Device: nRF52840_xxAA.
  * Target Interface: SWD.
  * RTT Control Block Detection: Auto.

* We can see the execution status of the two threads in SystemView.