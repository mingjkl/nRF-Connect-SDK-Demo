/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "sum.h"
#include "func_a.h"
#include "func_b.h"

int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	printk("sum_func(1, 2) = %d\n", sum_func(1, 2));
	func_a();
	func_b();
	return 0;
}
