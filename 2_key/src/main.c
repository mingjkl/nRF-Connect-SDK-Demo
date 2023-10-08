/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.c
 * @brief This file contains the code for a Zephyr and HAL API based application that toggles two LEDs using two push buttons.
 *
 * The application uses the Zephyr and HAL APIs to configure and set the state of two GPIO pins to turn on two LEDs.
 * It also configures the GPIO pin interrupt for one of the push buttons to trigger on the rising edge and initializes a callback function to be called when the interrupt is triggered.
 * The other push button is polled for its state and the state of the LED is toggled accordingly.
 *
 * The application can be compiled and run on a Zephyr or HAL API based board.
 */

#include <zephyr/kernel.h>

#define ZEPHYR_API

#ifdef ZEPHYR_API

#include <zephyr/drivers/gpio.h>


#define LED_RED_NODE DT_ALIAS(ledr)
#define LED_GREEN_NODE DT_ALIAS(ledg)
#define KEY0_NODE DT_ALIAS(button0)
#define KEY1_NODE DT_ALIAS(button1)

static const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios); 
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec key0 = GPIO_DT_SPEC_GET(KEY0_NODE, gpios);
static const struct gpio_dt_spec key1 = GPIO_DT_SPEC_GET(KEY1_NODE, gpios);

static struct gpio_callback pin_cb_data;

void pin_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
        gpio_pin_toggle_dt(&led_green);
		printk("key1 pressed\n");
}


int main(void)
{

	int ret;

	printk("Hello World! %s\n", CONFIG_BOARD);

	/**
	 * Check if the devices are ready before proceeding with the code execution.
	 * If any of the devices are not ready, print an error message and return.
	 */
	
	if (!device_is_ready(led_red.port))
	{
		printk("Device led is not ready\n");
		return;
	}

	if(!device_is_ready(led_green.port))
	{
		printk("Device led is not ready\n");
		return;
	}

	if(!device_is_ready(key1.port))
	{
		printk("Device key is not ready\n");
		return;
	}

	if(!device_is_ready(key0.port))
	{
		printk("Device key is not ready\n");
		return;
	}

	/**
	 * @brief Configures and sets the state of two GPIO pins to turn on two LEDs.
	 *
	 * This function configures two GPIO pins as output pins and sets their state to high to turn on two LEDs.
	 * If there is an error in configuring the pins, an error message is printed and the function returns.
	 */

	ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT);
	if (ret < 0) {
		printk("Error %d: failed to configure LED device\n", ret);
		return;
	}

	ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT);
	if (ret < 0) {
		printk("Error %d: failed to configure LED device\n", ret);
		return;
	}

	gpio_pin_set_dt(&led_red, 1);
	gpio_pin_set_dt(&led_green, 1);
	

	/**
	 * Configure the GPIO pin interrupt for key0 to trigger on the rising edge.
	 * Initialize a callback function to be called when the interrupt is triggered.
	 * Add the callback function to the GPIO port for key0.
	 */

	ret = gpio_pin_configure_dt(&key0, GPIO_INPUT);
	if (ret < 0) {
		printk("Error %d: failed to configure KEY device\n", ret);
		return;
	}
	
	gpio_pin_interrupt_configure_dt(&key0, GPIO_INT_EDGE_RISING);
	gpio_init_callback(&pin_cb_data, pin_isr, BIT(key0.pin));
	gpio_add_callback(key0.port, &pin_cb_data);
	

	/**
	 * Configures the KEY1 device as an input GPIO pin.Polls the state of the pin and toggles the state of the LED.
	 */
	ret = gpio_pin_configure_dt(&key1, GPIO_INPUT);
	if (ret < 0) {
		printk("Error %d: failed to configure KEY device\n", ret);
		return;
	}

	/** 
	 * Poll the state of the pin and toggle the state of the LED.
	*/
	while (1) {
		if(gpio_pin_get_dt(&key1))
		{
			gpio_pin_set_dt(&led_red, 1);
		}
		else
		{
			gpio_pin_set_dt(&led_red, 0);
		}
		k_msleep(100);
	}
}

#endif

#ifdef HAL_API

#include "hal/nrf_gpio.h"
#include "hal/nrf_gpiote.h"
#include "nrfx_gpiote.h"

// led and key pin define
#define LED_RED_PIN NRF_GPIO_PIN_MAP(0,2)
#define KEY0_PIN NRF_GPIO_PIN_MAP(0,28)
#define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0,3)
#define KEY1_PIN NRF_GPIO_PIN_MAP(0,5)


// interrupt key handler
void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin == KEY1_PIN)
	{
		nrf_gpio_pin_toggle(LED_GREEN_PIN);
	}
}

int main(void)
{
	int ret;
	

	printk("Hello World! %s\n", CONFIG_BOARD);

	// init led pin
	nrf_gpio_cfg_output(LED_RED_PIN);
	nrf_gpio_cfg_output(LED_GREEN_PIN);

	// init poll key pin
	nrf_gpio_cfg_input(KEY0_PIN, NRF_GPIO_PIN_PULLUP);

	/**
	 * Initializes and configures a GPIOTE input pin for KEY1_PIN with a pull-up resistor and a low-to-high sense.
	 * Enables the GPIOTE input event for KEY1_PIN.
	 */
	nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
	in_config.pull = NRF_GPIO_PIN_PULLUP;
	ret = nrfx_gpiote_in_init(KEY1_PIN, &in_config, in_pin_handler);
	if(ret != NRFX_SUCCESS)
	{
		printk("Error %d: failed to configure KEY device\n", ret);
		return;
	}
	nrfx_gpiote_in_event_enable(KEY1_PIN, true);

	// detect poll key
	while (1) {
		if(nrf_gpio_pin_read(KEY0_PIN))
		{
			nrf_gpio_pin_set(LED_RED_PIN);
		}
		else
		{
			nrf_gpio_pin_clear(LED_RED_PIN);
		}
		k_msleep(100);
	}
}

#endif