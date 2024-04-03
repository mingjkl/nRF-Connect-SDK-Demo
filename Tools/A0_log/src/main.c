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

LOG_MODULE_REGISTER(test, LOG_LEVEL_DBG);



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
