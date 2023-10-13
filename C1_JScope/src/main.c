/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <SEGGER_RTT.h>

#include <math.h>


struct line_s
{
	int line_0;
	int line_1;
};


int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);

	uint32_t cnt = 0;
	char JS_RTT_UpBuffer[128];
	int JS_RTT_Channel = 1;

	struct line_s line;

	SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_u4i4", JS_RTT_UpBuffer, sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

	while(1)
	{

		cnt++;

		line.line_0 = cnt % 100;
		line.line_1 = sin((float)cnt / 180) * 100;

		SEGGER_RTT_Write(JS_RTT_Channel, &line, sizeof(line));
		
		k_sleep(K_MSEC(1));
	}

	return 0;
}
