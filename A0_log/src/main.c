/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.c
 * @brief This file contains a simple example of using the Zephyr 
 * logging API to print messages of different levels and to dump 
 * data in hex format.
 *
 * The code registers a module named "test" with the logging 
 * subsystem and prints messages of different levels using 
 * the LOG_INF, LOG_WRN, LOG_DBG, and LOG_ERR macros. 
 * It also dumps an array of data in hex format using 
 * the LOG_HEXDUMP_INF macro.
 */
#include <zephyr/kernel.h>


// note: kconfig defines CONFIG_LOG=y

#include <zephyr/logging/log.h>
#include <SEGGER_RTT.h>
#include <SEGGER_SYSVIEW.h>
#include <math.h>

LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);


#define T1_THREAD_STATCK_SIZE 1024
#define T1_THREAD_PRIORITY 5

extern void t1_thread(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(t1_thread_id, T1_THREAD_STATCK_SIZE, t1_thread, NULL, NULL, NULL, T1_THREAD_PRIORITY, 0, 0);


#define T2_THREAD_STATCK_SIZE 1024
#define T2_THREAD_PRIORITY 5

extern void t2_thread(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(t2_thread_id, T2_THREAD_STATCK_SIZE, t2_thread, NULL, NULL, NULL, T2_THREAD_PRIORITY, 0, 0);

#define T3_THREAD_STATCK_SIZE 10240
#define T3_THREAD_PRIORITY 4

extern void t3_thread(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(t3_thread_id, T3_THREAD_STATCK_SIZE, t3_thread, NULL, NULL, NULL, T3_THREAD_PRIORITY, 0, 0);



void t1_thread(void *arg1, void *arg2, void *arg3)
{
	uint32_t cnt = 0;


	while (1) {
		LOG_INF("t1_thread: %d", cnt++);
		k_sleep(K_MSEC(2000));
	}
}

void t2_thread(void *arg1, void *arg2, void *arg3)
{
	uint32_t cnt = 0;
	while(1)
	{
		cnt += 56;
		LOG_INF("t2_thread: %d", cnt);
		k_sleep(K_MSEC(1000));
	}
}

void t3_thread(void *arg1, void *arg2, void *arg3)
{
	uint32_t cnt = 0;
	char JS_RTT_UpBuffer[4096];
	int JS_RTT_Channel = 1;

	// SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScopeu4i4", JS_RTT_UpBuffer, sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);


	struct {
		int cnt;
		int cnt2;
	} data;

	data.cnt = data.cnt2 = 0;

	int pol1 = 0;
	int pol2 = 0;

	while(1)
	{

		if(pol1 == 0) data.cnt++;
		else data.cnt--;

		if(pol2 == 0) data.cnt2 += 2;
		else data.cnt2 -= 2;

		if(data.cnt == 100) pol1 = 1;
		if(data.cnt == 20) pol1 = 0;

		if(data.cnt2 == 80) pol2 = 1;
		if(data.cnt2 == -20) pol2 = 0;
		

		// SEGGER_RTT_Write(JS_RTT_Channel, &data, sizeof(data));

		k_sleep(K_MSEC(1));
	}
}



int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	LOG_INF("This is an INFO message.");
	LOG_WRN("This is a WARNING message.");
	LOG_DBG("This is a DEBUG message.");
	LOG_ERR("This is an ERROR message.");

	uint8_t data[100] = {0};
	uint8_t p;

	for (p = 0; p < 100; p++) {
		data[p] = p;
	}

	LOG_HEXDUMP_INF(data, 100, "This is a hexdump of data.");


	return 0;
}
