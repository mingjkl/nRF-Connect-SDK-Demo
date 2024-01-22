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

#define VALUE_REPEATS 1UL

nrf_pwm_values_common_t pwm_val []=
{
    1,2,3
};

int main(void)
{
	nrfx_err_t err_code;
	nrfx_pwm_t pwm0 = NRFX_PWM_INSTANCE(0);
	nrfx_pwm_config_t config0 = NRFX_PWM_DEFAULT_CONFIG(PWM_PIN_0,
														PWM_PIN_1,
														PWM_PIN_2,
														PWM_PIN_3);
	config0.base_clock = NRF_PWM_CLK_16MHz;
	config0.top_value = 4;
	err_code = nrfx_pwm_init(&pwm0, &config0, NULL, &pwm0);
	printk("PWM init: %08x\n", err_code);	

	nrf_pwm_sequence_t seq0 = {
		.values = {pwm_val},
		.length = NRFX_ARRAY_SIZE(pwm_val),
		.repeats = VALUE_REPEATS,
		.end_delay = 0
	};

	err_code = nrfx_pwm_simple_playback(&pwm0, &seq0, 1, NRFX_PWM_FLAG_LOOP);

	return 0;
}
