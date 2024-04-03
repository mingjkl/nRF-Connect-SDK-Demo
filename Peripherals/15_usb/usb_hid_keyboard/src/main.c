/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>
#include <hal/nrf_gpio.h>


#define DEBUG_LED_PIN NRF_GPIO_PIN_MAP(0, 13)
#define BTN_PIN NRF_GPIO_PIN_MAP(0, 11)

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const uint8_t hid_report_desc[] = HID_KEYBOARD_REPORT_DESC();
const struct device *hid_dev;

static enum usb_dc_status_code usb_status;
static uint8_t report[8] = {0};

static int hid_on_set_report_cb(const struct device *dev,
			 struct usb_setup_packet *setup, int32_t *len,
			 uint8_t **data);

static const struct hid_ops ops = {
	.set_report = hid_on_set_report_cb,
};


static void status_cb(enum usb_dc_status_code status, const uint8_t *param)
{
	usb_status = status;
}

void led_init(void)
{
	nrf_gpio_cfg_output(DEBUG_LED_PIN);
	nrf_gpio_pin_set(DEBUG_LED_PIN);
}

void led_set(bool on)
{
	nrf_gpio_pin_write(DEBUG_LED_PIN, !on);
}


static int hid_on_set_report_cb(const struct device *dev,
			 struct usb_setup_packet *setup, int32_t *len,
			 uint8_t **data)
{
	if(*len != 1) {
		return -EINVAL;
	}

	**data & HID_KBD_LED_CAPS_LOCK ? led_set(true) : led_set(false);

	return 0;
}

void btn_init(void)
{
	nrf_gpio_cfg_input(BTN_PIN, NRF_GPIO_PIN_PULLUP);
}
                 
int main(void)
{
	int ret;

	led_init();
	btn_init();

	hid_dev = device_get_binding("HID_0");
	if (hid_dev == NULL) {
		LOG_ERR("Cannot get USB HID Device");
		return 0;
	}

	usb_hid_register_device(hid_dev,
				hid_report_desc, sizeof(hid_report_desc),
				&ops);
	
	usb_hid_init(hid_dev);

	ret = usb_enable(status_cb);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return 0;
	}

	k_msleep(1000);
	
	while(1)
	{
		if(!nrf_gpio_pin_read(BTN_PIN))
		{
			report[2] = HID_KEY_5;
			ret = hid_int_ep_write(hid_dev, report, sizeof(report), NULL);
		}
		else
		{
			report[2] = 0;
			ret = hid_int_ep_write(hid_dev, report, sizeof(report), NULL);
		}
		k_msleep(10);
	}

	return 0;
}
