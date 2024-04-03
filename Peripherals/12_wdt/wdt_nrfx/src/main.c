/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_wdt.h>
#include <hal/nrf_gpio.h>


#define DEBUG_PIN NRF_GPIO_PIN_MAP(0, 30)


static nrfx_wdt_t wdt = NRFX_WDT_INSTANCE(0);

static void wdt_handler(void)
{
	for(uint8_t i = 0; i < 10; i++)
	{
		nrf_gpio_pin_set(DEBUG_PIN);
		k_busy_wait(1);
		nrf_gpio_pin_clear(DEBUG_PIN);
		k_busy_wait(1);
	}
}

uint8_t wdt_channel_id;

int main(void)
{
	nrfx_wdt_config_t config = NRFX_WDT_DEFAULT_CONFIG;
	config.reload_value = 5000;
	nrfx_wdt_init(&wdt, &config, wdt_handler);

	nrf_gpio_cfg_output(DEBUG_PIN);
	nrf_gpio_pin_clear(DEBUG_PIN);

	IRQ_DIRECT_CONNECT(WDT_IRQn, IRQ_PRIO_LOWEST, NRFX_WDT_INST_HANDLER_GET(0), 0);
	nrfx_wdt_channel_alloc(&wdt, &wdt_channel_id);
	nrfx_wdt_enable(&wdt);

	printk("This is a wdt demo\n");

	while(1)
	{
		// nrfx_wdt_channel_feed(&wdt, wdt_channel_id);
		k_sleep(K_MSEC(1000));
		
	}
	
	return 0;
}
