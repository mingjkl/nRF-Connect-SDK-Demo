/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/kernel/thread.h>

#define THREAD_1_STACK_SIZE 500
#define THREAD_1_PRIORITY 5

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

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
								 

int main(void)
{
	LOG_INF("Thread demo");
	return 0;
}
