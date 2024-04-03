/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_i2s.h>
#include <hal/nrf_gpio.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_audio.h>



LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define I2S_SCK_PIN NRF_GPIO_PIN_MAP(0, 31)
#define I2S_LRCK_PIN NRF_GPIO_PIN_MAP(0, 30)
#define I2S_MCK_PIN NRF_GPIO_PIN_MAP(0, 29)
#define I2S_SDOUT_PIN NRF_GPIO_PIN_MAP(0, 28)
#define I2S_SDIN_PIN NRF_GPIO_PIN_MAP(0, 4)

#define I2S_DATA_BLOCK_WORDS    48

static uint32_t m_buffer_rx[2][I2S_DATA_BLOCK_WORDS];
static uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

static uint8_t i2s_buf_sw = 0;

static nrfx_i2s_buffers_t i2s_data_buffers = {
	.p_tx_buffer = m_buffer_tx[0],
	.p_rx_buffer = m_buffer_rx[0],
};

const struct device *hs_dev;



static void i2s_handler(nrfx_i2s_buffers_t const * p_released, uint32_t status)
{
	uint8_t i;
	uint32_t ret;

	if(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)
	{
		i2s_buf_sw = i2s_buf_sw ? 0 : 1;

		i2s_data_buffers.p_tx_buffer = m_buffer_tx[i2s_buf_sw];
		i2s_data_buffers.p_rx_buffer = m_buffer_rx[i2s_buf_sw];

		nrfx_i2s_next_buffers_set(&i2s_data_buffers);
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



void i2s_init(void)
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
	config.mck_setup = NRF_I2S_MCK_32MDIV21;

	
	nrfx_i2s_init(&config, i2s_handler);
	IRQ_DIRECT_CONNECT(I2S_IRQn, IRQ_PRIO_LOWEST, nrfx_i2s_irq_handler, 0);


	i2s_data_buffers.p_tx_buffer = m_buffer_tx[0];
	i2s_data_buffers.p_rx_buffer = m_buffer_rx[0];

	nrfx_i2s_start(&i2s_data_buffers, I2S_DATA_BLOCK_WORDS, 0);
}

static void data_received(const struct device *dev,
			  struct net_buf *buffer,
			  size_t size)
{
	int ret;
	int i;

	if (!buffer || !size) {
		/* This should never happen */
		return;
	}

	uint8_t temp_data_buffer[192];
	uint16_t audio_r[48];
	uint16_t audio_l[48];

	for(i = 0; i < buffer->size; i++)
	{
		temp_data_buffer[i] = net_buf_pull_u8(buffer);
	}

	for(i = 0; i < 48; i++)
	{
		audio_r[i] = temp_data_buffer[4 * i];
		audio_r[i] |= temp_data_buffer[4 * i + 1] << 8;
		audio_l[i] = temp_data_buffer[4 * i + 2];
		audio_l[i] |= temp_data_buffer[4 * i + 3] << 8;
	}

	for(i = 0; i < 48; i++)
	{
		m_buffer_tx[i2s_buf_sw ? 0 : 1][i] = (audio_r[i] << 16)|(audio_l[i]);
	}


	net_buf_unref(buffer);

}

static void feature_update(const struct device *dev,
			   const struct usb_audio_fu_evt *evt)
{
	LOG_DBG("Control selector %d for channel %d updated",
		evt->cs, evt->channel);
	switch (evt->cs) {
	case USB_AUDIO_FU_MUTE_CONTROL:
	default:
		break;
	}
}

static const struct usb_audio_ops ops = {
	.data_received_cb = data_received,
	.feature_update_cb = feature_update,

};

int main(void)
{
	int ret;
	
	i2s_init();

	hs_dev = DEVICE_DT_GET_ONE(usb_audio_hs);
	if (!device_is_ready(hs_dev)) {
		LOG_ERR("Device USB Headset is not ready");
		return 0;
	}

	usb_audio_register(hs_dev, &ops);
	ret = usb_enable(NULL);
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return 0;
	}

	LOG_INF("USB enabled");

	return 0;
}
