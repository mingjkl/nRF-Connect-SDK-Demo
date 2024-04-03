/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <hal/nrf_gpio.h>


#define DEBUG_PIN NRF_GPIO_PIN_MAP(0, 29)

static const struct device *wdt = DEVICE_DT_GET(DT_NODELABEL(wdt0));

int wdt_channel_id;

static void wdt_callback(const struct device *dev, int channel_id)
{
	for(uint8_t i = 0; i < 10; i++)
	{
		nrf_gpio_pin_toggle(DEBUG_PIN);
	}
}

int main(void)
{
	printk("Watchdog sample application\n");

	nrf_gpio_cfg_output(DEBUG_PIN);
	nrf_gpio_pin_clear(DEBUG_PIN);

	if (!device_is_ready(wdt)) {
		printk("Watchdog device %s is not ready\n", wdt->name);
		return 1;
	}

	struct wdt_timeout_cfg wdt_config = {
		.window.min = 0,		// ms
		.window.max = 5000,		// ms
		.callback = wdt_callback,
		.flags = WDT_FLAG_RESET_SOC,
	};
	wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);

	if (wdt_channel_id < 0) {
		printk("Watchdog install failed %d\n", wdt_channel_id);
		return 0;
	}
	else
	{
		printk("Watchdog Channel ID: %d\n", wdt_channel_id);
	}

	wdt_setup(wdt, NULL);
	
	while(1)
	{
		// wdt_feed(wdt, wdt_channel_id);	
		k_sleep(K_MSEC(1000));
	}
	return 0;
}
