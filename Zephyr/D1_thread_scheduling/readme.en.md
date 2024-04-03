## Key code
    ```c
        k_sched_lock() and k_sched_unlock() are used to lock the scheduler
        k_sleep() is used to put the thread to sleep
        k_msleep() is used to put the thread to sleep for a specified number of milliseconds
        k_usleep() is used to put the thread to sleep for a specified number of microseconds
        k_busy_wait() is used to busy wait for a specified number of nanoseconds, without putting the thread to sleep
        k_yield() is used to yield the processor to another thread of the same priority
        k_cpu_idle() is used to put the CPU to sleep
        k_cpu_atomic_idle() is used to put the CPU to sleep, but only if the CPU is not in an atomic context
    ```