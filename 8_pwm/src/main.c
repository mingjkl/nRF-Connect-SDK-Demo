/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_pwm.h>
#include <hal/nrf_gpio.h>

#define PWM_PIN_0 NRF_GPIO_PIN_MAP(0, 28)
#define PWM_PIN_1 NRF_GPIO_PIN_MAP(0, 29)
#define PWM_PIN_2 NRF_GPIO_PIN_MAP(0, 30)
#define PWM_PIN_3 NRF_GPIO_PIN_MAP(0, 31)


unsigned int frequency_to_topValue(unsigned int frequency)
{
	unsigned int topValue = 0;
	topValue = 16000000 / frequency;
	return topValue;
}

unsigned int dutyCycle_to_pwmValue(unsigned int dutyCycle, unsigned int topValue)
{
	unsigned int pwmValue = 0;
	pwmValue = (dutyCycle * topValue) / 100;
	return pwmValue;
}

nrf_pwm_values_individual_t pwm_duty;
nrf_pwm_values_common_t pwm_duty_common[10];

int main(void)
{

	// Configure PWM0
	// individual channels

	nrfx_err_t err_code;
	nrfx_pwm_t pwm0 = NRFX_PWM_INSTANCE(0);
	nrfx_pwm_config_t config0 = NRFX_PWM_DEFAULT_CONFIG(PWM_PIN_0,
														PWM_PIN_1,
														NULL,
														NULL);

	unsigned int freq = 10000;

	config0.base_clock = NRF_PWM_CLK_16MHz;
	config0.top_value = frequency_to_topValue(freq);
	config0.load_mode = NRF_PWM_LOAD_INDIVIDUAL;
	
	err_code = nrfx_pwm_init(&pwm0, &config0, NULL, &pwm0);


	pwm_duty.channel_0 = dutyCycle_to_pwmValue(50, frequency_to_topValue(freq));
	pwm_duty.channel_1 = dutyCycle_to_pwmValue(0, frequency_to_topValue(freq));

	nrf_pwm_sequence_t seq0 = {
		.values.p_individual = &pwm_duty,
		.length = NRF_PWM_VALUES_LENGTH(pwm_duty),
		.repeats = 1,
		.end_delay = 0
	};

	err_code = nrfx_pwm_simple_playback(&pwm0, &seq0, 1, NRFX_PWM_FLAG_LOOP);


	// Configure PWM1
	// common channels

	nrfx_pwm_t pwm1 = NRFX_PWM_INSTANCE(1);
	nrfx_pwm_config_t config1 = NRFX_PWM_DEFAULT_CONFIG(PWM_PIN_2,
														PWM_PIN_3,
														NULL,
														NULL);
	unsigned int freq1 = 1000;

	config1.base_clock = NRF_PWM_CLK_16MHz;
	config1.top_value = frequency_to_topValue(freq1);
	config1.load_mode = PWM_DECODER_LOAD_Common;

	err_code = nrfx_pwm_init(&pwm1, &config1, NULL, &pwm1);

	for(uint8_t i = 0; i < 10; i++)
	{
		pwm_duty_common[i] = dutyCycle_to_pwmValue(10 * i, frequency_to_topValue(freq1));
	}

	nrf_pwm_sequence_t seq1 = {
		.values.p_common = pwm_duty_common,
		.length = NRF_PWM_VALUES_LENGTH(pwm_duty_common),
		.repeats = 0,
		.end_delay = 0
	};

	err_code = nrfx_pwm_simple_playback(&pwm1, &seq1, 1, NRFX_PWM_FLAG_LOOP);


	// dynamic duty cycle

	uint8_t dynamic_duty = 1;
	uint8_t dynamic_duty_dir = 0;

	while(1)
	{
		
		if(dynamic_duty_dir == 0)
		{
			dynamic_duty+=10;
		}
		else
		{
			dynamic_duty-=10;
		}

		if(dynamic_duty > 90)
		{
			dynamic_duty_dir = 1;
		}
		else if(dynamic_duty < 10)
		{
			dynamic_duty_dir = 0;
		}
		
		pwm_duty.channel_1 = dutyCycle_to_pwmValue(dynamic_duty, frequency_to_topValue(freq));
		k_msleep(100);
	}

	return 0;
}
