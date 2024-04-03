/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_temp.h>

static void temp_handler(int32_t temperature)
{
	int32_t temp_celsius = nrfx_temp_calculate(temperature);
	printk("Temperature: %d\n", temp_celsius);
	
}

int main(void)
{
	nrfx_temp_config_t config = NRFX_TEMP_DEFAULT_CONFIG;

	nrfx_temp_init(&config, temp_handler);
	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TEMP), IRQ_PRIO_LOWEST, nrfx_temp_irq_handler, 0);
	while(1)
	{
		nrfx_temp_measure();
		k_sleep(K_MSEC(100));
	}
	return 0;
}
