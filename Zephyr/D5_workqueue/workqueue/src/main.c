/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>


#define MY_STACK_SIZE 512
#define MY_PRIORITY 5

K_THREAD_STACK_DEFINE(my_stack_area, MY_STACK_SIZE);

struct k_work_q my_work_q;

void this_is_my_work_handler_A(struct k_work *work)
{
	printk("This is my work handler A\n");
}

void this_is_my_work_handler_B(struct k_work *work)
{
	printk("This is my work handler B\n");
}

K_WORK_DEFINE(my_work_A, this_is_my_work_handler_A);
K_WORK_DEFINE(my_work_B, this_is_my_work_handler_B);

int main(void)
{
	k_work_queue_init(&my_work_q);
	k_work_queue_start(&my_work_q,
					   my_stack_area,
					   K_THREAD_STACK_SIZEOF(my_stack_area),
					   MY_PRIORITY,
					   NULL);

	k_work_submit_to_queue(&my_work_q, &my_work_A);
	k_work_submit_to_queue(&my_work_q, &my_work_B);

					   
	return 0;
}
