/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <hal/nrf_gpio.h>

#define EEPROM_ADDR 0x50

#define I2C_SDA_PIN NRF_GPIO_PIN_MAP(0, 31)
#define I2C_SCL_PIN NRF_GPIO_PIN_MAP(0, 30)
#define I2C_SDA_PIN_SET_WRITE() nrf_gpio_cfg_output(I2C_SDA_PIN)
#define I2C_SDA_PIN_SET_READ() nrf_gpio_cfg_input(I2C_SDA_PIN, NRF_GPIO_PIN_NOPULL)

#define I2C_DELAY_US	150


void i2c_delay_us(uint32_t us)
{
	k_busy_wait(us);
}

void I2C_SDA_PIN_set(uint8_t state)
{
	I2C_SDA_PIN_SET_WRITE();
	if (state)
		nrf_gpio_pin_set(I2C_SDA_PIN);
	else
		nrf_gpio_pin_clear(I2C_SDA_PIN);
}

void I2C_SCL_PIN_set(uint8_t state)
{
	if (state)
		nrf_gpio_pin_set(I2C_SCL_PIN);
	else
		nrf_gpio_pin_clear(I2C_SCL_PIN);
}

uint8_t I2C_SDA_PIN_read(void)
{
	I2C_SDA_PIN_SET_READ();
	return nrf_gpio_pin_read(I2C_SDA_PIN);
}

void i2c_start(void)
{
	I2C_SDA_PIN_set(1);
	I2C_SCL_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SDA_PIN_set(0);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SCL_PIN_set(0);
}

void i2c_stop(void)
{
	I2C_SDA_PIN_set(0);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SCL_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SDA_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
}

void i2c_write_bit(uint8_t bit)
{
	I2C_SDA_PIN_set(bit);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SCL_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SCL_PIN_set(0);
}

uint8_t i2c_read_bit(void)
{
	uint8_t bit;

	I2C_SDA_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
	I2C_SCL_PIN_set(1);
	i2c_delay_us(I2C_DELAY_US);
	bit = I2C_SDA_PIN_read();
	I2C_SCL_PIN_set(0);
	return bit;
}

void i2c_write_byte(uint8_t data)
{
	uint8_t i;


	for (i = 0; i < 8; i++) {
		i2c_write_bit(data & 0x80);
		data <<= 1;
	}
	i2c_read_bit();
}

uint8_t i2c_read_byte(uint8_t ack)
{
	uint8_t i;
	uint8_t data = 0;

	for (i = 0; i < 8; i++) {
		data <<= 1;
		data |= i2c_read_bit();
	}
	i2c_write_bit(ack);
	return data;
}

void i2c_init(void)
{
	nrf_gpio_cfg_output(I2C_SDA_PIN);
	nrf_gpio_cfg_output(I2C_SCL_PIN);
}

void eeprom_write_byte(uint8_t addr, uint8_t data)
{
	i2c_start();
	i2c_write_byte(EEPROM_ADDR << 1);
	i2c_write_byte(addr);
	i2c_write_byte(data);
	i2c_stop();
}

void eeprom_read_byte(uint8_t addr, uint8_t *data)
{
	i2c_start();
	i2c_write_byte(EEPROM_ADDR << 1);
	i2c_write_byte(addr);
	i2c_delay_us(I2C_DELAY_US);
	i2c_start();
	i2c_write_byte((EEPROM_ADDR << 1) | 1);
	*data = i2c_read_byte(1);
	i2c_stop();
}

void eeprom_write_array(uint8_t addr, uint8_t *data, uint8_t len)
{
	uint8_t i;

	i2c_start();
	i2c_write_byte(EEPROM_ADDR << 1);
	i2c_write_byte(addr);
	for (i = 0; i < len; i++) {
		i2c_write_byte(data[i]);
	}
	i2c_stop();
}

void eeprom_read_array(uint8_t addr, uint8_t *data, uint8_t len)
{
	uint8_t i;

	i2c_start();
	i2c_write_byte(EEPROM_ADDR << 1);
	i2c_write_byte(addr);
	i2c_delay_us(I2C_DELAY_US);
	i2c_start();
	i2c_write_byte((EEPROM_ADDR << 1) | 1);
	for (i = 0; i < len; i++) {
		data[i] = i2c_read_byte(i == len - 1);
	}
	i2c_stop();
}

int main(void)
{
	i2c_init();

	// Write and read single byte

	eeprom_write_byte(30, 0x55);
	k_busy_wait(1000);
	uint8_t data;

	eeprom_read_byte(30, &data);
	k_busy_wait(1000);


	// Write and read array

	uint8_t write_array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint8_t read_array[10];

	eeprom_write_array(0, write_array, 10);
	k_busy_wait(1000);
	eeprom_read_array(0, read_array, 10);
	k_busy_wait(1000);

	return 0;
}
