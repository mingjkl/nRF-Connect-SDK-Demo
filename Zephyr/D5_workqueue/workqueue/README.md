## 简介
当你需要在不阻塞当前执行线程的情况下执行任务时，工作队列特别有用。例如，在中断服务例程（ISR）中，你可能需要执行一个耗时的操作，但不希望阻塞中断处理。这时，可以将任务添加到工作队列，由一个不同的线程来处理。

这是一个关于work queue的demo，demo中定义了一个work queue，定义了两个work。然后将两个work分别放入work queue中并执行。

这里演示的是work queue的基本使用方法，另外一个配合[workqueue_uart](D5_workqueue\workqueue_uart)使用更直观。

## 代码介绍
* 定义一个栈空间，用于存放work queue中的work
```
    K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);
```
* 定义一个work queue
```
    struct k_work_q my_work_q;
```
* 初始化work queue
```
    k_work_queue_init(&my_work_q);
	k_work_queue_start(&my_work_q,
					   my_stack_area,
					   K_THREAD_STACK_SIZEOF(my_stack_area),
					   MY_PRIORITY,
					   NULL);
```
就是定义了我们的work queue的的栈空间，栈大小，优先级等内容；
* 定义work
```
    K_WORK_DEFINE(my_work_A, this_is_my_work_handler_A);
```
* 补充work的实现
```
    void this_is_my_work_handler_A(struct k_work *work)
    {
        printk("this is my work handler A\n");
    }
```
* 将work放入work queue中
```
    k_work_submit_to_queue(&my_work_q, &my_work_A);
```
* 编译和烧录
* 查看串口打印