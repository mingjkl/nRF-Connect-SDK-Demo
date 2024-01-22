/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_rtc.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(rtc, LOG_LEVEL_DBG);

static nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(0);

static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
	if(int_type == NRFX_RTC_INT_COMPARE0)
	{

		nrfx_rtc_counter_clear(&rtc);
		nrfx_rtc_cc_set(&rtc, 0, 1000, true);
		LOG_INF("RTC COMPARE0 interrupt\n");
	}
	
	if(int_type == NRFX_RTC_INT_OVERFLOW)
	{
		LOG_INF("RTC OVERFLOW interrupt\n");
	}

}

int main(void)
{
	
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
	
	config.tick_latency = NRFX_RTC_US_TO_TICKS(20000, 32768);

	nrfx_rtc_init(&rtc, &config, rtc_handler);

	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_RTC_INST_HANDLER_GET(0), 0);
	
	nrfx_rtc_cc_set(&rtc, 0, 1000, true);
	nrfx_rtc_overflow_enable(&rtc, true);
	nrfx_rtc_enable(&rtc);

	return 0;
}
