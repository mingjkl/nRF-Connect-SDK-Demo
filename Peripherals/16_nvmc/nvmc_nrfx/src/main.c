/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_nvmc.h>
#include <zephyr/logging/log.h>	

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define FLASH_ADDRESS 0x000f8000

int main(void)
{
	nrfx_err_t err_code;

	uint32_t flash_size = nrfx_nvmc_flash_size_get();
	uint32_t flash_page_count = nrfx_nvmc_flash_page_count_get();
    uint32_t page_size = nrfx_nvmc_flash_page_size_get();

	uint32_t wr_address = FLASH_ADDRESS;
    
    LOG_INF("Flash page size: %d bytes", page_size);
	LOG_INF("Total flash size: %d bytes", flash_size);
	LOG_INF("Total flash page count: %d", flash_page_count);

	// Read data from flash

	uint8_t *data_to_read = (uint8_t *)wr_address;

	LOG_INF("Data read from flash address: 0x%08x", (uint32_t)data_to_read);
	LOG_HEXDUMP_INF(data_to_read, 16, "Read data");

	// Erase flash page
	if (data_to_read[0] != 0xFF)
	{
		LOG_INF("Erasing flash page");

		// check page address is aligned to page size

		if (wr_address % page_size != 0)
		{
			LOG_ERR("Flash page address is not aligned to page size");
			return -1;
		}

		err_code = nrfx_nvmc_page_erase(wr_address);
		if (err_code != NRFX_SUCCESS)
		{
			LOG_ERR("Failed to erase flash page: 0x%08x", err_code);
		}

		while(nrfx_nvmc_write_done_check() == false)
		{
			// wait for erase to complete
		}
	}

	// Read data from flash after erase
	LOG_HEXDUMP_INF(data_to_read, 16, "Read data");

	// Prepare data to write to flash

	uint8_t data_to_write[16];
	for(int i = 0; i < sizeof(data_to_write); i++)
	{
		data_to_write[i] = 0x10 + i;
	}

	// Write data to flash
	LOG_INF("Write data to flash");
	nrfx_nvmc_bytes_write(wr_address, data_to_write, sizeof(data_to_write));
	LOG_INF("Waiting for write to complete");
	while(nrfx_nvmc_write_done_check() == false)
	{
		// wait for write to complete
	}

	// Read data from flash again
	LOG_HEXDUMP_INF(data_to_read, 16, "Read data");

	return 0;
}
