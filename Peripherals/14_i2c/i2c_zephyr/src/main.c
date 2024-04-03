/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/i2c.h>

#define EEPROM_ADDR 0x50

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));


void eeprom_write(uint8_t addr, uint8_t *data, uint8_t len)
{
	struct i2c_msg msgs[2];
	msgs[0].buf = &addr;
	msgs[0].len = 1;
	msgs[0].flags = I2C_MSG_WRITE;
	msgs[1].buf = data;
	msgs[1].len = len;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	i2c_transfer(i2c_dev, msgs, 2, EEPROM_ADDR);
}

void eeprom_read(uint8_t addr, uint8_t *data, uint8_t len)
{
	struct i2c_msg msgs[2];
	msgs[0].buf = &addr;
	msgs[0].len = 1;
	msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;
	msgs[1].buf = data;
	msgs[1].len = len;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	i2c_transfer(i2c_dev, msgs, 2, EEPROM_ADDR);
}

int main(void)
{	
	if(!device_is_ready(i2c_dev)){
		LOG_ERR("I2C device is not ready");
		return -1;
	}
	else{
		LOG_INF("I2C device is ready");
	}

	uint8_t data_buffer_tx[5] = {0x00, 0x01, 0x02, 0x03, 0x04};

	eeprom_write(0xC1, data_buffer_tx, 5);
	LOG_HEXDUMP_INF(data_buffer_tx, 5, "Data written to EEPROM");

	k_busy_wait(5000);

	uint8_t data_buffer_rx[10];
	eeprom_read(0xC1, data_buffer_rx, 5);
	LOG_HEXDUMP_INF(data_buffer_rx, 5, "Data read from EEPROM");


	return 0;
}
