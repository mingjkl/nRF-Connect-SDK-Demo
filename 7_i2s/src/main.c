/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2s.h>

#define SAMPLE_FREQUENCY    48000
#define SAMPLE_BIT_WIDTH    16
#define BYTES_PER_SAMPLE    sizeof(int16_t)
#define NUMBER_OF_CHANNELS  2
/* Such block length provides an echo with the delay of 100 ms. */
#define SAMPLES_PER_BLOCK   ((SAMPLE_FREQUENCY / 10) * NUMBER_OF_CHANNELS)
#define INITIAL_BLOCKS      2
#define TIMEOUT             1000

#define BLOCK_SIZE  (BYTES_PER_SAMPLE * SAMPLES_PER_BLOCK)
#define BLOCK_COUNT (INITIAL_BLOCKS + 2)
K_MEM_SLAB_DEFINE_STATIC(mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);


LOG_MODULE_REGISTER(i2s, LOG_LEVEL_DBG);	

const struct device *const i2s_dev= DEVICE_DT_GET(DT_NODELABEL(i2s0));
static K_SEM_DEFINE(toggle_transfer, 1, 1);

static bool configure_streams(const struct device *i2s_dev_rx,
			      const struct device *i2s_dev_tx,
			      const struct i2s_config *config)
{
	int ret;

	if (i2s_dev_rx == i2s_dev_tx) {
		ret = i2s_configure(i2s_dev_rx, I2S_DIR_BOTH, config);
		if (ret == 0) {
			return true;
		}
		/* -ENOSYS means that the RX and TX streams need to be
		 * configured separately.
		 */
		if (ret != -ENOSYS) {
			LOG_INF("Failed to configure streams: %d\n", ret);
			return false;
		}
	}

	ret = i2s_configure(i2s_dev_rx, I2S_DIR_RX, config);
	if (ret < 0) {
		LOG_INF("Failed to configure RX stream: %d\n", ret);
		return false;
	}

	ret = i2s_configure(i2s_dev_tx, I2S_DIR_TX, config);
	if (ret < 0) {
		LOG_INF("Failed to configure TX stream: %d\n", ret);
		return false;
	}

	return true;
}

static bool prepare_transfer(const struct device *i2s_dev_rx,
			     const struct device *i2s_dev_tx)
{
	int ret;

	for (int i = 0; i < INITIAL_BLOCKS; ++i) {
		void *mem_block;

		ret = k_mem_slab_alloc(&mem_slab, &mem_block, K_NO_WAIT);
		if (ret < 0) {
			LOG_INF("Failed to allocate TX block %d: %d\n", i, ret);
			return false;
		}

		memset(mem_block, 0, BLOCK_SIZE);

		ret = i2s_write(i2s_dev_tx, mem_block, BLOCK_SIZE);
		if (ret < 0) {
			LOG_INF("Failed to write block %d: %d\n", i, ret);
			return false;
		}
	}

	return true;
}

static bool trigger_command(const struct device *i2s_dev_rx,
			    const struct device *i2s_dev_tx,
			    enum i2s_trigger_cmd cmd)
{
	int ret;

	if (i2s_dev_rx == i2s_dev_tx) {
		ret = i2s_trigger(i2s_dev_rx, I2S_DIR_BOTH, cmd);
		if (ret == 0) {
			return true;
		}
		/* -ENOSYS means that commands for the RX and TX streams need
		 * to be triggered separately.
		 */
		if (ret != -ENOSYS) {
			LOG_INF("Failed to trigger command %d: %d\n", cmd, ret);
			return false;
		}
	}

	ret = i2s_trigger(i2s_dev_rx, I2S_DIR_RX, cmd);
	if (ret < 0) {
		LOG_INF("Failed to trigger command %d on RX: %d\n", cmd, ret);
		return false;
	}

	ret = i2s_trigger(i2s_dev_tx, I2S_DIR_TX, cmd);
	if (ret < 0) {
		LOG_INF("Failed to trigger command %d on TX: %d\n", cmd, ret);
		return false;
	}

	return true;
}

int main(void)
{

	LOG_INF("This is i2s demo");

	if (!device_is_ready(i2s_dev)) {
		LOG_INF("%s is not ready\n", i2s_dev->name);
		return 0;
	}
	else
	{
		LOG_INF("%s is ready\n", i2s_dev->name);
	}

	struct i2s_config config;
	config.word_size = SAMPLE_BIT_WIDTH;
	config.channels = NUMBER_OF_CHANNELS;
	config.format = I2S_FMT_DATA_FORMAT_I2S;
	config.options = I2S_OPT_BIT_CLK_MASTER |
					 I2S_OPT_FRAME_CLK_MASTER;
	config.frame_clk_freq = SAMPLE_FREQUENCY;
	config.mem_slab = &mem_slab;
	config.block_size = BLOCK_SIZE;
	config.timeout = TIMEOUT;

	if (!configure_streams(i2s_dev, i2s_dev, &config))
	{
		LOG_INF("Failed to configure streams\n");
		return 0;
	}
	else
	{
		LOG_INF("configure streams success\n");
	}

	for (;;) {
		// k_sem_take(&toggle_transfer, K_FOREVER);

		if (!prepare_transfer(i2s_dev, i2s_dev)) {
			return 0;
		}

		if (!trigger_command(i2s_dev, i2s_dev,
				     I2S_TRIGGER_START)) {
			return 0;
		}

		LOG_INF("Streams started\n");

		while (k_sem_take(&toggle_transfer, K_NO_WAIT) != 0) {
			void *mem_block;
			uint32_t block_size;
			int ret;

			ret = i2s_read(i2s_dev, &mem_block, &block_size);
			if (ret < 0) {
				LOG_INF("Failed to read data: %d\n", ret);
				break;
			}

			// process_block_data(mem_block, SAMPLES_PER_BLOCK);

			ret = i2s_write(i2s_dev, mem_block, block_size);
			if (ret < 0) {
				LOG_INF("Failed to write data: %d\n", ret);
				break;
			}
		}

		if (!trigger_command(i2s_dev, i2s_dev,
				     I2S_TRIGGER_DROP)) {
			return 0;
		}

		LOG_INF("Streams stopped\n");
	}
	
	

	return 0;
}
