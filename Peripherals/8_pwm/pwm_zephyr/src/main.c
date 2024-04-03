/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>


static const struct device *pwm_dev = DEVICE_DT_GET(DT_NODELABEL(pwm0));

int main(void)
{

	int ret;

	if(!device_is_ready(pwm_dev)){
		printk("PWM device is not ready\n");
		return -1;
	}
	else{
		printk("PWM device is ready\n");
	}

	uint32_t freq = 1000;						   // Hz
	uint32_t pulse_width = 25;					   // percentage
	uint32_t period = 1000000000 / freq;		   // ns
	uint32_t pulse = (period * pulse_width) / 100; // ns

	ret = pwm_set(pwm_dev, 1, period, pulse, NULL);
	ret = pwm_set(pwm_dev, 2, period, pulse * 2, NULL);
	printk("PWM set returned %d\n", ret);


	return 0;
}
