## Introduction
This demo introduces how to create a thread.

## Key Code
* Add the necessary header files
    ```c
    #include <zephyr/kernel.h>
    #include <zephyr/kernel/thread.h>
    ```

* Create a thread
    ```c
    #define THREAD_1_STACK_SIZE 500
    #define THREAD_1_PRIORITY 5

    extern void thread_1(void *arg1, void *arg2, void *arg3);

    K_THREAD_DEFINE(thread_1_id, THREAD_1_STACK_SIZE, 
                thread_1, NULL, NULL, NULL,
                THREAD_1_PRIORITY, 0, 0);

    void thread_1(void *arg1, void *arg2, void *arg3)
    {
        while (1) {
            k_msleep(1000);
            LOG_INF("Thread 1");
        }
    }
    ```

    Here, a thread called thread_1 is created with a stack size of 500 bytes and a priority of 5. The entry function for the thread is thread_1, and the thread's parameters are NULL.

    With this, the thread thread_1 is successfully created.