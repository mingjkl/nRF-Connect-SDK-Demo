/**
 * @file main.c
 * @brief This file contains the code for toggling an LED using GPIO on Zephyr OS or Nordic Semiconductor's HAL API.
 *
 * The code checks if the LED device is ready and configures it as an output. It then toggles the LED on and off
 * with a delay of 500ms using GPIO pin set and clear functions.
 *
 * @note This code is written for educational purposes only.
 */

#include <zephyr/kernel.h>

#define ZEPHYR_API

#ifdef ZEPHYR_API

#include <zephyr/drivers/gpio.h>

#define LED_RED_NODE DT_ALIAS(ledb)

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios); 

int main(void)
{
	int ret;

	printk("Hello World! %s\n", CONFIG_BOARD);

	if (!device_is_ready(led_red.port))
	{
		printk("Device is not ready\n");
		return;
	}

	ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT);
	if (ret < 0) {
		printk("Error %d: failed to configure LED device\n", ret);
		return;
	}

	while (1) {
		gpio_pin_set_dt(&led_red, 1);
		k_msleep(500);
		gpio_pin_set_dt(&led_red, 0);
		k_msleep(500);
	}
	return 0;
}

#endif

#ifdef HAL_API

#include "hal/nrf_gpio.h"

#define LED_RED_PIN NRF_GPIO_PIN_MAP(0,2)

int main(void)
{
	int ret;

	printk("Hello World! %s\n", CONFIG_BOARD);

	nrf_gpio_cfg_output(LED_RED_PIN);

	while (1) {
		nrf_gpio_pin_set(LED_RED_PIN);
		k_msleep(500);
		nrf_gpio_pin_clear(LED_RED_PIN);
		k_msleep(500);
	}
	return 0;
}

#endif
