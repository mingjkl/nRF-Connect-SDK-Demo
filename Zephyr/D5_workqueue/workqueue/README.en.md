## Introduction
Work queues are particularly useful when you need to perform tasks without blocking the current thread of execution. For example, in an interrupt service routine (ISR), you may need to perform a time-consuming operation but do not want to block interrupt processing. At this point, you can add the task to a work queue and have it processed by a different thread.

This is a demo of a work queue, where a work queue is defined and two works are defined. the two works are then put into the work queue and executed separately.

This demonstrates the basic use of a work queue, and the other one is more intuitive to use with [workqueue_uart](D5_workqueue\workqueue_uart).

## Code introduction
* Define a stack space to hold the work in the work queue.
```
    K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);
```
* Define a work queue
```
    struct k_work_q my_work_q;
```
* Initialize the work queue
```
    k_work_queue_init(&my_work_q);
	k_work_queue_start(&my_work_q,
					   my_stack_area,
					   k_THREAD_STACK_SIZEOF(my_stack_area),
					   MY_PRIORITY, NULL); &my_work_q, my_stack_area
					   NULL);
```
This defines the stack space, stack size, priority, etc. of our work queue;
* `` Define work
```
    K_WORK_DEFINE(my_work_A, this_is_my_work_handler_A); ```
```
* Supplement the work implementation
```
    void this_is_my_work_handler_A(struct k_work *work)
    {
        printk("this is my work handler A\n");
    }
```
* Place work in the work queue.
```
    k_work_submit_to_queue(&my_work_q, &my_work_A);
```
* Compile and burn
* View serial port printout
