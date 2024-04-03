## 简介
这个demo使用Zephyr RTOS API进行nvs操作，保护数据的读写和擦除。demo中设备上电后，会读取并打印nvs中的数据，然后擦除nvs中的数据，再写入新的数据。然后再次读取并打印nvs中的数据。

## 关键代码
* 在`prj.conf`中进行kconfig配置
    ```c
    CONFIG_FLASH=y
    CONFIG_FLASH_PAGE_LAYOUT=y
    CONFIG_NVS=y
    CONFIG_LOG=y
    ```

* 加入必要的头文件
    ```c
    #include <zephyr/device.h>
    #include <zephyr/drivers/flash.h>
    #include <zephyr/storage/flash_map.h>
    #include <zephyr/fs/nvs.h>
    #include <zephyr/logging/log.h>
    ```

* flash的一些宏定义
    ```c
    #define NVS_PARTITION		storage_partition
    #define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
    #define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)
    ```
    这里面的`storage_partition`是在`prj.conf`中配置的nvs分区。其中`FIXED_PARTITION_DEVICE`和`FIXED_PARTITION_OFFSET`是宏定义，用于获取nvs分区的设备和偏移量。

* 定义一个nvs实例
    ```c
    static struct nvs_fs fs;
    ```

* 设置nvs实例的设备和偏移量
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

* 获取page信息
    ```c
    ret = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &fs_page_info);
    ```

* 配置sector信息
    ```c
    fs.sector_size = fs_page_info.size;
	fs.sector_count = 3U;
    ```
    这里面的`fs_page_info.size`是获取到的page大小，`3U`是sector的数量。这里的sector数量是自己定义的，可以根据实际情况进行调整。

* 挂载nvs
    ```c
    ret = nvs_mount(&fs);
    ```

* 读取nvs中的数据
    ```c
    ret = nvs_read(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
    ```
    这里面的`STORAGE_DATA_ID`是自己定义的数据ID，同来标记我们储存数据的位置，`storage_data_buf`是用于存储读取到的数据的缓冲区。如果找不到`STORAGE_DATA_ID`，则会返回错误。

* 写入nvs数据
    ```c
    nvs_write(&fs, STORAGE_DATA_ID, storage_data_buf, sizeof(storage_data_buf));
    ```
    这里面的`STORAGE_DATA_ID`是给要写入数据标记的ID，`storage_data_buf`是要写入的数据。

* 删除nvs数据
    ```c
    nvs_delete(&fs, STORAGE_DATA_ID);
    ```
    这里面的`STORAGE_DATA_ID`是要删除的数据的ID。
