/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>

#define T1_THREAD_STATCK_SIZE 1024
#define T1_THREAD_PRIORITY 5
extern void t1_thread(void *arg1, void *arg2, void *arg3);
K_THREAD_DEFINE(t1_thread_id, T1_THREAD_STATCK_SIZE, t1_thread, NULL, NULL, NULL, T1_THREAD_PRIORITY, 0, 0);


#define T2_THREAD_STATCK_SIZE 1024
#define T2_THREAD_PRIORITY 4
extern void t2_thread(void *arg1, void *arg2, void *arg3);
K_THREAD_DEFINE(t2_thread_id, T2_THREAD_STATCK_SIZE, t2_thread, NULL, NULL, NULL, T2_THREAD_PRIORITY, 0, 0);


void t1_thread(void *arg1, void *arg2, void *arg3)
{
	uint32_t cnt = 0;

	printk("Start thread 1\r\n");

	while (1) {
		for(cnt = 0; cnt < 100; cnt++)
		{
			k_busy_wait(100);
		}
		k_sleep(K_MSEC(200));
	}
}

void t2_thread(void *arg1, void *arg2, void *arg3)
{
	uint32_t cnt = 0;

	printk("Start thread 2\r\n");

	while(1)
	{
		for(cnt = 0; cnt < 100; cnt++)
		{
			k_busy_wait(100);
		}
		k_sleep(K_MSEC(100));
	}
}


int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	return 0;
}
