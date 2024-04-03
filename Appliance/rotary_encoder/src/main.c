/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// WARNING: This project builds for the nRF52840 DK board and SDK 2.4.0

#include <stdio.h>
#include <hal/nrf_gpio.h>
#include <nrfx_gpiote.h>
#include <hal/nrf_gpiote.h>
#include <zephyr/kernel.h>

#define ENCODER_PIN_A NRF_GPIO_PIN_MAP(0, 30)
#define ENCODER_PIN_B NRF_GPIO_PIN_MAP(0, 31)

int encoder_value = 0;

void set_pin_trigger(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

static void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin != ENCODER_PIN_A) return;
	
	if(action == NRF_GPIOTE_POLARITY_LOTOHI)
	{
		encoder_value += nrf_gpio_pin_read(ENCODER_PIN_B) == 0 ? 1 : -1;
		set_pin_trigger(ENCODER_PIN_A, NRF_GPIOTE_POLARITY_HITOLO);
		printk("ev=%d\n", encoder_value);
	}
	else if(action == NRF_GPIOTE_POLARITY_HITOLO)
	{
		encoder_value += nrf_gpio_pin_read(ENCODER_PIN_B) == 0 ? -1 : 1;
		set_pin_trigger(ENCODER_PIN_A, NRF_GPIOTE_POLARITY_LOTOHI);
		printk("ev=%d\n", encoder_value);
	}
}

void encoder_pin_init(void)
{
	nrf_gpio_cfg_input(ENCODER_PIN_A, NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(ENCODER_PIN_B, NRF_GPIO_PIN_PULLDOWN);

	uint8_t init_level = nrf_gpio_pin_read(ENCODER_PIN_A);

	if(init_level == 0)
	{
		set_pin_trigger(ENCODER_PIN_A, NRF_GPIOTE_POLARITY_LOTOHI);
	}
	else
	{
		set_pin_trigger(ENCODER_PIN_A, NRF_GPIOTE_POLARITY_HITOLO);
	}

}

void set_pin_trigger(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);

	if(action == NRF_GPIOTE_POLARITY_HITOLO)
	{
		in_config.sense = NRF_GPIOTE_POLARITY_HITOLO;
	}
	

	in_config.pull = NRF_GPIO_PIN_PULLDOWN;
	nrfx_gpiote_in_init(ENCODER_PIN_A, &in_config, in_pin_handler);
	nrfx_gpiote_in_event_enable(ENCODER_PIN_A, true);
}

int main(void)
{
	encoder_pin_init();
	return 0;
}
