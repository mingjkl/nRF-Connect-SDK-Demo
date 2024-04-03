/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <hal/nrf_gpio.h>

#define LCD_BLK_PIN NRF_GPIO_PIN_MAP(0, 29)   // Backlight pin

const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

int main(void)
{

	nrf_gpio_cfg_output(LCD_BLK_PIN);
	nrf_gpio_pin_set(LCD_BLK_PIN);

	if (device_is_ready(display_dev)) {
		printf("Display device is ready\n");
	} else {
		printf("Display device is not ready\n");
	}

	display_blanking_off(display_dev);

	lv_obj_t *label;
	label = lv_label_create(lv_scr_act());
	
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

	char display_text[8];
	uint16_t counter = 0;

	while(1)
	{
		k_sleep(K_MSEC(10));
		sprintf(display_text, "%d", counter++);
		lv_label_set_text(label, display_text);
		lv_task_handler();
	}

	return 0;
}
