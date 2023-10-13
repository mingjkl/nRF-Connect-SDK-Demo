## Introduction
This is an example that demonstrates the usage of Logging.

## Steps
* Create a "hello world" demo.
* Add the following configuration in prj.conf:
    ```
    CONFIG_LOG=y
    ```
* Add the header file in main.c:
    ```
    #include <zephyr/logging/log.h>
    ```
* Register a logger:
    ```
    LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);
    ```
* Use the logger: \
  Several usage examples are demonstrated in the main function.