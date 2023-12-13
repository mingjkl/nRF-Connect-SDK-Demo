/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/logging/log.h>
#include "nrfx_spim.h"

nrfx_spim_t spi_instance = NRFX_SPIM_INSTANCE(4);
LOG_MODULE_REGISTER(spi_driver, LOG_LEVEL_DBG);


#define LCD_I2C_CS_PIN      NRF_GPIO_PIN_MAP(0,30)
#define LCD_I2C_SDA_PIN     NRF_GPIO_PIN_MAP(1,2)
#define LCD_I2C_CLK_PIN     NRF_GPIO_PIN_MAP(1,3)
#define LCD_BACKLIGHT_PIN     NRF_GPIO_PIN_MAP(1,12)

int SPI_init(void)
{
    nrfx_spim_config_t spi_config;

    spi_config.sck_pin = LCD_I2C_CLK_PIN;
    spi_config.mosi_pin = LCD_I2C_SDA_PIN;
    spi_config.miso_pin = 0xFF;
    spi_config.ss_pin = LCD_I2C_CS_PIN;


    spi_config.irq_priority = 72;
    spi_config.orc = 0xFF;
    spi_config.frequency = SPIM_FREQUENCY_FREQUENCY_M2;
    spi_config.mode = NRF_SPIM_MODE_0;
    spi_config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    spi_config.miso_pull = GPIO_PIN_CNF_PULL_Pullup;
    spi_config.ss_active_high = false;


    nrfx_spim_init(&spi_instance, &spi_config, NULL, NULL);

    LOG_INF("SPI Init done\r\n");

}

void SPI_write(uint8_t *data, uint16_t len)
{
    nrfx_spim_xfer_desc_t spi_tx_buff = NRFX_SPIM_XFER_TX(data, len);
    nrfx_spim_xfer(&spi_instance, &spi_tx_buff, 0);
}

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD);
	return 0;
}
