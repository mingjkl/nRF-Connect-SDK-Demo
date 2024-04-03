/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_twi.h>
#include <hal/nrf_gpio.h>
#include <zephyr/logging/log.h>

#define EEPROM_ADDR 0x50

#define SDA_PIN NRF_GPIO_PIN_MAP(0, 31)
#define SCL_PIN NRF_GPIO_PIN_MAP(0, 30)

static const nrfx_twi_t twi = NRFX_TWI_INSTANCE(0);

LOG_MODULE_REGISTER(eeprom, LOG_LEVEL_DBG);

void twi_handler(nrfx_twi_evt_t const * p_event, void * p_context) 
{
	switch (p_event->type) {
		case NRFX_TWI_EVT_DONE:
			/* Transfer completed */
			break;
		case NRFX_TWI_EVT_ADDRESS_NACK:
			/* NACK received after sending the address */
			break;
		case NRFX_TWI_EVT_DATA_NACK:
			/* NACK received after sending a data byte */
			break;
		default:
			break;
	}
}


uint8_t eeprom_write_bytes(uint8_t addr, uint8_t date) 
{
	uint8_t buffer[2] = {addr, date};
	nrfx_twi_xfer_desc_t xfer_desc = {
		.type = NRFX_TWI_XFER_TX,
		.address = EEPROM_ADDR,
		.primary_length = sizeof(buffer),
		.p_primary_buf = buffer,
	};

	while(nrfx_twi_is_busy(&twi)){};
	nrfx_twi_xfer(&twi, &xfer_desc, 0);
	return 0;
}

uint8_t eeprom_write_array(uint8_t addr, uint8_t *data, uint8_t len) 
{
	uint8_t buffer[len + 1];
	buffer[0] = addr;
	for (int i = 0; i < len; i++) {
		buffer[i + 1] = data[i];
	}
	nrfx_twi_xfer_desc_t xfer_desc = {
		.type = NRFX_TWI_XFER_TX,
		.address = EEPROM_ADDR,
		.primary_length = sizeof(buffer),
		.p_primary_buf = buffer,
	};

	while(nrfx_twi_is_busy(&twi)){};
	nrfx_twi_xfer(&twi, &xfer_desc, 0);
	return 0;
}


uint8_t eeprom_read_bytes(uint8_t addr) 
{
	uint8_t addr_buffer[1] = {addr};
	uint8_t data_buffer[1];

	nrfx_twi_xfer_desc_t xfer_desc = {
		.type = NRFX_TWI_XFER_TXRX,
		.address = EEPROM_ADDR,
		.primary_length = sizeof(addr_buffer),
		.p_primary_buf = addr_buffer,
		.secondary_length = sizeof(data_buffer),
		.p_secondary_buf = data_buffer,
	};

	
	nrfx_twi_xfer(&twi, &xfer_desc, 0);
	while(nrfx_twi_is_busy(&twi)){};
	return data_buffer[0];
}

uint8_t eeprom_read_array(uint8_t addr, uint8_t *data, uint8_t len) 
{
	uint8_t addr_buffer[1] = {addr};
	uint8_t data_buffer[len];

	nrfx_twi_xfer_desc_t xfer_desc = {
		.type = NRFX_TWI_XFER_TXRX,
		.address = EEPROM_ADDR,
		.primary_length = sizeof(addr_buffer),
		.p_primary_buf = addr_buffer,
		.secondary_length = sizeof(data_buffer),
		.p_secondary_buf = data_buffer,
	};

	
	nrfx_twi_xfer(&twi, &xfer_desc, 0);
	while(nrfx_twi_is_busy(&twi)){};
	for (int i = 0; i < len; i++) {
		data[i] = data_buffer[i];
	}
	return 0;
}


int main(void)
{
	
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG(SCL_PIN, SDA_PIN);
	nrfx_twi_init(&twi, &config, twi_handler, NULL);
	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TWI_INST_GET(0)), IRQ_PRIO_LOWEST,
                       NRFX_TWI_INST_HANDLER_GET(0), 0);
	nrfx_twi_enable(&twi);

	
	eeprom_write_bytes(0x00, 0x55);
	LOG_INF("EEPROM write byte 0x55 to address 0x00");

	k_busy_wait(5000);
	
	uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
	LOG_INF("EEPROM write array 0x01, 0x02, 0x03, 0x04 to address 0xA0");
	eeprom_write_array(0xA0, data, 4);

	k_busy_wait(5000);
	uint8_t data_byte = eeprom_read_bytes(0xA2);
	LOG_INF("EEPROM read byte from address 0xA2: 0x%x", data_byte);
	k_busy_wait(5000);

	uint8_t data_read[30];
	eeprom_read_array(0x00, data_read, 30);
	LOG_HEXDUMP_INF(data_read, 30, "EEPROM read array from address 0xA0: ");

	return 0;
}
