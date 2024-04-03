/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/logging/log.h>

static struct nvs_fs fs;

LOG_MODULE_REGISTER(main);

#define NVS_PARTITION		storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define STORAGE_DATA_ID	9

uint8_t storage_data_buf[16];

int main(void)
{
	int ret;
	static struct flash_pages_info fs_page_info;

	fs.flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs.flash_device)) {
		LOG_ERR("Flash device %s is not ready", fs.flash_device->name);
		return 0;
	}
	else
	{
		LOG_INF("Flash device %s is ready", fs.flash_device->name);
	}

	// Set the flash offset
	fs.offset = NVS_PARTITION_OFFSET;

	// Get the page info, including the size of the page and the number of pages
	ret = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &fs_page_info);

	fs.sector_size = fs_page_info.size;
	fs.sector_count = 3U;

	ret = nvs_mount(&fs);
	if (ret < 0) {
		LOG_ERR("Error mounting nvs: %d", ret);
		return 0;
	}
	else
	{
		LOG_INF("NVS mounted");
	}

	// Read the data from the NVS
	ret = nvs_read(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
	
	if(ret < 0)
	{
		LOG_ERR("Error reading data from NVS: %d", ret);
	}
	
	LOG_HEXDUMP_INF(storage_data_buf, sizeof(storage_data_buf), "Read data from NVS");
	
	// Delete the data from the NVS
	LOG_INF("Deleting data from NVS");
	nvs_delete(&fs, STORAGE_DATA_ID);

	// Read the data from the NVS after deleting it
	ret = nvs_read(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
	if(ret < 0)
	{
		LOG_ERR("Error reading data from NVS: %d", ret);
	}
	
	// Write the data to the NVS
	LOG_INF("Writing data to NVS");
	for(int i = 0; i < sizeof(storage_data_buf); i++)
	{
		storage_data_buf[i] = 0xE0 + i;
	}
	nvs_write(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));


	// Read the data from the NVS again
	uint8_t read_data[sizeof(storage_data_buf)];
	ret = nvs_read(&fs, STORAGE_DATA_ID, read_data, sizeof(read_data));
	LOG_HEXDUMP_INF(read_data, sizeof(read_data), "Read data from NVS");

	return 0;
}
