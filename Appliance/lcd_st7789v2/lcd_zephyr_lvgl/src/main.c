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
#include <zephyr/kernel/thread.h>
#include "ui.h"

#define LCD_BLK_PIN NRF_GPIO_PIN_MAP(0, 29)   // Backlight pin

#define UI_THREAD_STACK_SIZE 40960
#define UI_THREAD_PRIORITY 2

const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

extern void ui_thread(void *arg1, void *arg2, void *arg3);

K_THREAD_DEFINE(ui_thread_id, UI_THREAD_STACK_SIZE, 
				ui_thread, NULL, NULL, NULL,
				UI_THREAD_PRIORITY, 0, 0);

void ui_thread(void *arg1, void *arg2, void *arg3)
{
	nrf_gpio_cfg_output(LCD_BLK_PIN);
	nrf_gpio_pin_set(LCD_BLK_PIN);

	if (device_is_ready(display_dev)) {
		printf("Display device is ready\n");
	} else {
		printf("Display device is not ready\n");
	}

	ui_init();
	lv_task_handler();
	display_blanking_off(display_dev);

	lv_chart_refresh(ui_Chart1);

	lv_chart_series_t *series1 = lv_chart_add_series(ui_Chart1,
													 lv_palette_main(LV_PALETTE_RED),
													 LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_update_mode(ui_Chart1, LV_CHART_UPDATE_MODE_SHIFT);
	uint8_t i = 0;

	lv_keyboard_set_mode(ui_Keyboard1, LV_KEYBOARD_MODE_NUMBER);

	while (1) {
		lv_chart_set_next_value(ui_Chart1, series1, i);
		i += 10;
		if(i > 100) i = 0;

		k_sleep(K_MSEC(10));
		lv_task_handler();
	}
}
		

int main(void)
{


	while(1)
	{
		// lv_chart_set_next(ui_Chart1, series1, 100);
		k_sleep(K_MSEC(1000));
	}

	return 0;
}
