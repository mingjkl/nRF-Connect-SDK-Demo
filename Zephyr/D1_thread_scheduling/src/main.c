/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/kernel/thread.h>

// k_sched_lock() and k_sched_unlock() are used to lock the scheduler
// k_sleep() is used to put the thread to sleep
// k_msleep() is used to put the thread to sleep for a specified number of milliseconds
// k_usleep() is used to put the thread to sleep for a specified number of microseconds
// k_busy_wait() is used to busy wait for a specified number of nanoseconds, without putting the thread to sleep
// k_yield() is used to yield the processor to another thread of the same priority
// k_cpu_idle() is used to put the CPU to sleep
// k_cpu_atomic_idle() is used to put the CPU to sleep, but only if the CPU is not in an atomic context

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define THREAD_1_STACK_SIZE 500
#define THREAD_1_PRIORITY 5

#define THREAD_2_STACK_SIZE 500
#define THREAD_2_PRIORITY 6

#define THREAD_3_STACK_SIZE 500
#define THREAD_3_PRIORITY 7


extern void thread_1(void *arg1, void *arg2, void *arg3);
extern void thread_2(void *arg1, void *arg2, void *arg3);
extern void thread_3(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(thread_1_id, THREAD_1_STACK_SIZE, 
				thread_1, NULL, NULL, NULL,
				THREAD_1_PRIORITY, 0, 0);

K_THREAD_DEFINE(thread_2_id, THREAD_2_STACK_SIZE,
				thread_2, NULL, NULL, NULL,
				THREAD_2_PRIORITY, 0, 0);

K_THREAD_DEFINE(thread_3_id, THREAD_3_STACK_SIZE,
				thread_3, NULL, NULL, NULL,
				THREAD_3_PRIORITY, 0, 0);


void thread_1(void *arg1, void *arg2, void *arg3)
{
	while (1) {
		k_msleep(1000);
		LOG_INF("Thread 1");
	}
}

void thread_2(void *arg1, void *arg2, void *arg3)
{
	while (1) {
		k_msleep(2000);
		LOG_INF("Thread 2");
	}
}

void thread_3(void *arg1, void *arg2, void *arg3)
{
	while (1) {
		k_msleep(3000);
		LOG_INF("Thread 3");
	}
}


int main(void)
{
	LOG_INF("Thread demo");
	return 0;
}
