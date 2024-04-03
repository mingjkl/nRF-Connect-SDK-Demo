## 关键代码

* `k_sched_lock()` 和 `k_sched_unlock()` 用于锁定调度器。
* `k_sleep()` 用于使线程休眠。
* `k_msleep()` 用于使线程休眠指定的毫秒数。
* `k_usleep()` 用于使线程休眠指定的微秒数。
* `k_busy_wait()` 用于忙等待指定的纳秒数，不会使线程休眠。
* `k_yield()` 用于让出处理器给另一个同优先级的线程。
* `k_cpu_idle()` 用于使CPU进入休眠状态。
* `k_cpu_atomic_idle()` 用于在CPU不处于原子上下文时使CPU进入休眠状态。

