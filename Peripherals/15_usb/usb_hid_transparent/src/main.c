/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static const uint8_t custom_hid_report_desc[] = {
	0x06, 0x00, 0xff, // Usage Page = 0xFF00 (Vendor Defined Page 1)
	0x09, 0x00,		  // Usage (0x00)
	0xA1, 0x01,		  // Collection (Application)
	0x09, 0x01,		  // Usage (0x01)
	0x15, 0x00,		  // Logical Minimum (0)
	0x26, 0xFF, 0x00, // Logical Maximum (255)
	0x75, 0x08,		  // Report Size (8)
	0x95, 0x40,		  // Report Count (64)
	0x81, 0x00,		  // Input
	0x09, 0x02,		  // Usage (0x02)
	0x15, 0x00,		  // Logical Minimum (0)
	0x26, 0xFF, 0x00, // Logical Maximum (255)
	0x75, 0x08,		  // Report Size (8)
	0x95, 0x40,		  // Report Count (64)
	0x91, 0x00,		  // Output
	0xC0			  // End Collection
};


const struct device *hid_dev;
static enum usb_dc_status_code usb_status;

static uint8_t tx_report[64] = {0};
static uint8_t rx_report[64] = {0};

static void hid_int_out_ready_cb(const struct device *dev)
{
	LOG_DBG("HID Interrupt OUT endpoint ready");
	hid_int_ep_read(hid_dev, rx_report, sizeof(rx_report), NULL);
	LOG_HEXDUMP_DBG(rx_report, sizeof(rx_report), "HID Interrupt OUT report");
}

static void hid_int_in_ready(const struct device *dev)
{
	// LOG_DBG("HID Interrupt IN endpoint ready");
}

static const struct hid_ops ops = {
	.int_out_ready = hid_int_out_ready_cb,
	.int_in_ready = hid_int_in_ready,
};

static void status_cb(enum usb_dc_status_code status, const uint8_t *param)
{
	usb_status = status;
}

int main(void)
{
	uint32_t ret;

	hid_dev = device_get_binding("HID_0");
	if (hid_dev == NULL) {
		LOG_ERR("Cannot get USB HID Device");
		return 0;
	}

	usb_hid_register_device(hid_dev,
				custom_hid_report_desc, sizeof(custom_hid_report_desc),
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
		tx_report[7]++;
		tx_report[23]++;

		hid_int_ep_write(hid_dev, tx_report, sizeof(tx_report), NULL);
		k_msleep(1000);
	}

	return 0;
}
