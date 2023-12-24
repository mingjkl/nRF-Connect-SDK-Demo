/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "zephyr/logging/log.h"
#include "nrfx_saadc.h"
#include <zephyr/kernel.h>


LOG_MODULE_REGISTER(adc, LOG_LEVEL_DBG);

#define CHANNEL_COUNT 1

static nrfx_saadc_channel_t single_channel = NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN3, 0);
static nrf_saadc_value_t buffer[CHANNEL_COUNT];

int adc_init(void)
{
	nrfx_err_t err;
	err = nrfx_saadc_init(NRFX_SAADC_DEFAULT_CONFIG_IRQ_PRIORITY);
	err = nrfx_saadc_channel_config(&single_channel);
	uint32_t channels_mask = nrfx_saadc_channels_configured_get();

	LOG_INF("ADC channels mask: %d\n", channels_mask);
	
	err = nrfx_saadc_simple_mode_set(channels_mask, NRF_SAADC_RESOLUTION_12BIT, NRF_SAADC_OVERSAMPLE_DISABLED, NULL);
	err = nrfx_saadc_buffer_set(buffer, CHANNEL_COUNT);
}

int adc_value_get(void)
{
	nrfx_err_t err;
	
	err = nrfx_saadc_offset_calibrate(NULL);
	err = nrfx_saadc_mode_trigger();
	return buffer[0];
}

int adc_to_voltage(int adc_value)
{
	return (adc_value * 3600) / 4096;
}

int main(void)
{
	LOG_INF("ADC Demo %s\n", CONFIG_BOARD);
	adc_init();
	
	while (1)
	{
		printk("vol=%d\r\n", adc_to_voltage(adc_value_get()));
		k_msleep(10);
	}
	
	return 0;
}
