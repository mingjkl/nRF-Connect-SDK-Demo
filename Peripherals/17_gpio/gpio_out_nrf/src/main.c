/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <hal/nrf_gpio.h>

#define LED_PIN NRF_GPIO_PIN_MAP(0, 13)

int main(void)
{
	nrf_gpio_cfg_output(LED_PIN);


	while(1)
	{
		nrf_gpio_pin_set(LED_PIN);
		k_sleep(K_MSEC(500));
		nrf_gpio_pin_clear(LED_PIN);
		k_sleep(K_MSEC(500));
	}

	return 0;
}
