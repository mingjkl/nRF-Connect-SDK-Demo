## Introduction
This demo utilizes Zephyr RTOS API for non-volatile storage (NVS) operations, enabling secure data read, write, and erase functionalities. Upon powering up the device, the demo reads and prints data from the NVS, erases the existing data, writes new data, and finally reads and prints the updated NVS data.

## Key Code Snippets
* Perform Kconfig configuration in `prj.conf`
    ```c
    CONFIG_FLASH=y
    CONFIG_FLASH_PAGE_LAYOUT=y
    CONFIG_NVS=y
    CONFIG_LOG=y
    ```

* Include necessary header files
    ```c
    #include <zephyr/device.h>
    #include <zephyr/drivers/flash.h>
    #include <zephyr/storage/flash_map.h>
    #include <zephyr/fs/nvs.h>
    #include <zephyr/logging/log.h>
    ```

* Define some flash macros
    ```c
    #define NVS_PARTITION		storage_partition
    #define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
    #define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)
    ```
    Here, `storage_partition` is the NVS partition configured in `prj.conf`. `FIXED_PARTITION_DEVICE` and `FIXED_PARTITION_OFFSET` are macros used to retrieve the NVS partition's device and offset.

* Define an NVS instance
    ```c
    static struct nvs_fs fs;
    ```

* Set the device and offset for the NVS instance
    ```c
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
    ```

* Get page information
    ```c
    ret = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &fs_page_info);
    ```

* Configure sector information
    ```c
    fs.sector_size = fs_page_info.size;
	fs.sector_count = 3U;
    ```
    Here, `fs_page_info.size` is the obtained page size, and `3U` represents the number of sectors. The sector count can be adjusted based on requirements.

* Mount the NVS
    ```c
    ret = nvs_mount(&fs);
    ```

* Read data from the NVS
    ```c
    ret = nvs_read(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
    ```
    Here, `STORAGE_DATA_ID` is a user-defined data ID used to locate the stored data, and `storage_data_buf` is the buffer to store the read data. An error is returned if `STORAGE_DATA_ID` is not found.

* Write data to the NVS
    ```c
    nvs_write(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
    ```
    Here, `STORAGE_DATA_ID` is the ID used to mark the data to be written, and `storage_data_buf` contains the data to be written.

* Delete data from the NVS
    ```c
    nvs_delete(&fs, STORAGE_DATA_ID);
    ```
    Here, `STORAGE_DATA_ID` is the ID of the data to be deleted.
"