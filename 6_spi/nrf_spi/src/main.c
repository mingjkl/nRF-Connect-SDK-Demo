/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/logging/log.h>
#include "nrfx_spim.h"
#include "hal/nrf_gpio.h"
#include <zephyr/kernel.h>

nrfx_spim_t spi_instance = NRFX_SPIM_INSTANCE(4);
LOG_MODULE_REGISTER(spi_driver, LOG_LEVEL_DBG);




#define MX25_CS_PIN      NRF_GPIO_PIN_MAP(0,11)
#define MX25_SCK_PIN     NRF_GPIO_PIN_MAP(0,8)
#define MX25_MOSI_PIN    NRF_GPIO_PIN_MAP(0,9)
#define MX25_MISO_PIN    NRF_GPIO_PIN_MAP(0,10)
#define MX25_WP_PIN      NRF_GPIO_PIN_MAP(0,15)
#define MX25_HOLD_PIN    NRF_GPIO_PIN_MAP(0,16)

// #define MX25_CS_PIN      NRF_GPIO_PIN_MAP(0,17)
// #define MX25_SCK_PIN     NRF_GPIO_PIN_MAP(0,19)
// #define MX25_MOSI_PIN    NRF_GPIO_PIN_MAP(0,20)
// #define MX25_MISO_PIN    NRF_GPIO_PIN_MAP(0,21)
// #define MX25_WP_PIN      NRF_GPIO_PIN_MAP(0,22)
// #define MX25_HOLD_PIN    NRF_GPIO_PIN_MAP(0,23)

int MX25_init(void)
{
    nrf_gpio_cfg_output(MX25_CS_PIN);
    nrf_gpio_cfg_output(MX25_WP_PIN);
    nrf_gpio_cfg_output(MX25_HOLD_PIN);

    nrf_gpio_pin_set(MX25_CS_PIN);
    nrf_gpio_pin_clear(MX25_WP_PIN);
    nrf_gpio_pin_set(MX25_HOLD_PIN);

    SPI_init();

    LOG_INF("MX25 Init done\r\n");
}

int SPI_init(void)
{
    nrfx_spim_config_t spi_config;

    spi_config.sck_pin = MX25_SCK_PIN;
    spi_config.mosi_pin = MX25_MOSI_PIN;
    spi_config.miso_pin = MX25_MISO_PIN;
    spi_config.ss_pin = MX25_CS_PIN;


    spi_config.irq_priority = 72;
    spi_config.orc = 0xFF;
    spi_config.frequency = SPIM_FREQUENCY_FREQUENCY_M2;
    spi_config.mode = NRF_SPIM_MODE_3;
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

void SPI_read(uint8_t *data, uint16_t len)
{
    nrfx_spim_xfer_desc_t spi_rx_buff = NRFX_SPIM_XFER_RX(data, len);
    nrfx_spim_xfer(&spi_instance, &spi_rx_buff, 0);
}

#define MX25_CMD_WRITE_ENABLE 0x06
#define MX25_CMD_PAGE_PROGRAM 0x02
#define MX25_CMD_READ        0x03

void MX25_write_enable(void)
{
    uint8_t cmd = MX25_CMD_WRITE_ENABLE;
    nrf_gpio_pin_clear(MX25_CS_PIN);
    SPI_write(&cmd, 1);
    nrf_gpio_pin_set(MX25_CS_PIN);
}

void MX25_write_byte(uint32_t addr, uint8_t data)
{
    uint8_t cmd[4];
    cmd[0] = MX25_CMD_PAGE_PROGRAM;
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    MX25_write_enable();

    nrf_gpio_pin_clear(MX25_CS_PIN);
    SPI_write(cmd, 4);
    SPI_write(&data, 1);
    nrf_gpio_pin_set(MX25_CS_PIN);
}

void MX25_read(uint32_t addr, uint8_t *data, uint16_t len)
{
    uint8_t cmd[4];
    cmd[0] = MX25_CMD_READ;
    cmd[1] = (addr >> 16) & 0xFF;
    cmd[2] = (addr >> 8) & 0xFF;
    cmd[3] = addr & 0xFF;

    nrf_gpio_pin_clear(MX25_CS_PIN);
    SPI_write(cmd, 4);
    SPI_write(data, len);
    nrf_gpio_pin_set(MX25_CS_PIN);
}

int main(void)
{
	LOG_INF("Hello World! %s\n", CONFIG_BOARD);

    MX25_init();
    uint8_t temp = 0;
    MX25_read(0x001234, &temp, 1);
    LOG_INF("before temp %d", temp);
    MX25_write_byte(0x001234, 0x55);
    k_msleep(100);
    MX25_read(0x001234, &temp, 1);
    LOG_INF("after temp %d", temp);
	return 0;
}
