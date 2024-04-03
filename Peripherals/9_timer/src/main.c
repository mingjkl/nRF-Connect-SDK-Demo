/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_timer.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(timer, LOG_LEVEL_DBG);

static void timer_handler(nrf_timer_event_t event_type, void * p_context)
{
	if(event_type == NRF_TIMER_EVENT_COMPARE0)
	{
		LOG_INF("Timer event compare 0");
	}
}

int main(void)
{
	nrfx_timer_t timer = NRFX_TIMER_INSTANCE(0);
	nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG;
	
	config.frequency = NRF_TIMER_FREQ_16MHz;
	config.bit_width = NRF_TIMER_BIT_WIDTH_32;

	nrfx_timer_init(&timer, &config, timer_handler);

	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_TIMER_INST_HANDLER_GET(0), 0);

	nrfx_timer_clear(&timer);

	uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer, 5000);

	nrfx_timer_extended_compare(&timer, NRF_TIMER_CC_CHANNEL0, desired_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
	nrfx_timer_enable(&timer);


	while(1)
	{
		k_msleep(1000);
	}

	return 0;
}
