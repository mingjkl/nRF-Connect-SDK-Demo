/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_i2s.h>
#include <hal/nrf_gpio.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define I2S_SCK_PIN NRF_GPIO_PIN_MAP(0, 31)
#define I2S_LRCK_PIN NRF_GPIO_PIN_MAP(0, 30)
#define I2S_MCK_PIN NRF_GPIO_PIN_MAP(0, 29)
#define I2S_SDOUT_PIN NRF_GPIO_PIN_MAP(0, 28)
#define I2S_SDIN_PIN NRF_GPIO_PIN_MAP(0, 4)

#define I2S_DATA_BLOCK_WORDS    128

uint32_t m_buffer_rx[2][I2S_DATA_BLOCK_WORDS];
uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

uint8_t i2s_buf_sw = 0;


static void i2s_handler(nrfx_i2s_buffers_t const * p_released, uint32_t status)
{
	if(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)
	{
		i2s_buf_sw = i2s_buf_sw ? 0 : 1;

		nrfx_i2s_buffers_t next_buffers = {
			.p_tx_buffer = m_buffer_tx[i2s_buf_sw],
			.p_rx_buffer = m_buffer_rx[i2s_buf_sw],
		};
		nrfx_i2s_next_buffers_set(&next_buffers);
	}

	if(status & NRFX_I2S_STATUS_TRANSFER_STOPPED)
	{
		nrfx_i2s_buffers_t init_buffers = {
		.p_tx_buffer = m_buffer_tx[0],
		.p_rx_buffer = m_buffer_rx[0],
	};
		nrfx_i2s_start(&init_buffers, I2S_DATA_BLOCK_WORDS, 0);
	}
}

int main(void)
{
	nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(I2S_SCK_PIN,
													   I2S_LRCK_PIN,
													   I2S_MCK_PIN,
													   I2S_SDOUT_PIN,
													   I2S_SDIN_PIN);
	config.channels = NRF_I2S_CHANNELS_STEREO;
	config.sample_width = NRF_I2S_SWIDTH_16BIT;
	config.alignment = NRF_I2S_ALIGN_LEFT;
	config.mode = NRF_I2S_MODE_MASTER;
	config.ratio = NRF_I2S_RATIO_32X;
	config.mck_setup = NRF_I2S_MCK_32MDIV23;

	
	nrfx_i2s_init(&config, i2s_handler);
	IRQ_DIRECT_CONNECT(I2S_IRQn, IRQ_PRIO_LOWEST, nrfx_i2s_irq_handler, 0);

	nrfx_i2s_buffers_t init_buffers = {
		.p_tx_buffer = m_buffer_tx[0],
		.p_rx_buffer = m_buffer_rx[0],
	};

	for(int i = 0; i < I2S_DATA_BLOCK_WORDS; i++)
	{
		m_buffer_tx[0][i] = 0x11223344;
		m_buffer_tx[1][i] = 0x00000000;
	}

	nrfx_i2s_start(&init_buffers, I2S_DATA_BLOCK_WORDS, 0);


	return 0;
}
