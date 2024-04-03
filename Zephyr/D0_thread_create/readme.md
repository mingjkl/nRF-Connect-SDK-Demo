## 简介
这个demo介绍了如何创建一个线程。

## 关键代码
* 添加必要的头文件
    ```c
    #include <zephyr/kernel.h>
    #include <zephyr/kernel/thread.h>
    ```

* 创建一个线程
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

    这里创建了一个线程thread_1，线程的栈大小为500字节，优先级为5。线程的入口函数为thread_1，线程的参数为NULL。

    至此，线程thread_1创建完成。