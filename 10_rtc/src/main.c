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

unsigned int freq_to_prescaler(unsigned int freq)
{
	return (unsigned int)round(32768.0 / freq) - 1;
}

unsigned int prescaler_to_freq(unsigned int prescaler)
{
	return (unsigned int)round(32768.0 / (prescaler + 1));
}

unsigned int overflow_us_to_ticks(unsigned int us, unsigned int freq)
{
	return (((us) * (freq)) / 1000000U);
}

unsigned int overflow_ms_to_ticks(unsigned int ms, unsigned int freq)
{
	return overflow_us_to_ticks((ms) * 1000U, freq);
}

unsigned int ticks_to_us(unsigned int ticks, unsigned int freq)
{
	return (((ticks) * 1000000U) / (freq));
}

static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
	if(int_type == NRFX_RTC_INT_COMPARE0)
	{
		unsigned int ticks =  ticks_to_us(nrfx_rtc_counter_get(&rtc), prescaler_to_freq(rtc.p_reg->PRESCALER));
		
		if(ticks > 1000)
			LOG_INF("RTC: %d ms\n", ticks / 1000);
		else
			LOG_INF("RTC: %d us\n", ticks);

		nrfx_rtc_counter_clear(&rtc);
	}
}

int main(void)
{
	
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;

	// frtc = 32.768 / (prescaler + 1)
	// prescaler = round(32.768 / frtc) - 1

	unsigned freq = 32;

	config.prescaler = freq_to_prescaler(freq);

	nrfx_rtc_init(&rtc, &config, rtc_handler);

	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(0)), IRQ_PRIO_LOWEST, NRFX_RTC_INST_HANDLER_GET(0), 0);
	
	nrfx_rtc_cc_set(&rtc, 0, overflow_ms_to_ticks(30000, freq), true);

	nrfx_rtc_enable(&rtc);

	return 0;
}
