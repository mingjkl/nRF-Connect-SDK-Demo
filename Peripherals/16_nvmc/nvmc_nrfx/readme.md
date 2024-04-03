## 简介
这是一个使用`nrfx_nvmc.h`库实现nrf52840的内部flash读写的简单例程。示例中先读取指定地址的数据，然后判断该地址的数据是否为`0xFF`，如果是，则写入数据，否则擦除该page后再写入数据。完成数据的读写后，再次读取数据并打印。

## 关键代码
* 在`prj.conf`中添加必要的配置
    ```c
    CONFIG_NRFX_NVMC=y
    CONFIG_FLASH=y
    ```

* 添加必要的头文件
    ```c
    #include <nrfx_nvmc.h>
    ```
* 获取page size
    ```c
    uint32_t page_size = nrfx_nvmc_flash_page_size_get();
    ```

* page擦除
    ```c
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
    ```
    `nrfx_nvmc_page_erase()`传入的是page的地址，所以需要保证page的地址是page size的整数倍。执行擦除操作后，需要等待擦除操作完成，否则会出现错误。

* 写入数据
    ```c
	nrfx_nvmc_bytes_write(wr_address, data_to_write, sizeof(data_to_write));
	while(nrfx_nvmc_write_done_check() == false)
	{
		// wait for write to complete
	}
    ```
    `nrfx_nvmc_bytes_write()`传入的是写入的地址和数据。执行写入操作后，需要等待写入操作完成，否则会出现错误。函数写入的是字节数据；如果写入`uint32_t`数据，可以考虑使用`nrfx_nvmc_words_write()`;

* 读取数据
    ```c
    uint8_t *data_to_read = (uint8_t *)wr_address;

	LOG_INF("Data read from flash address: 0x%08x", (uint32_t)data_to_read);
	LOG_HEXDUMP_INF(data_to_read, 16, "Read data");
    ```
    是的，直接读取地址就可以了。

* LOG内容
    ```c
    *** Booting Zephyr OS build v3.3.99-ncs1-1 ***
    [00:00:00.252,838] <inf> main: Flash page size: 4096 bytes
    [00:00:00.252,868] <inf> main: Total flash size: 1048576 bytes
    [00:00:00.252,868] <inf> main: Total flash page count: 256
    [00:00:00.252,899] <inf> main: Data read from flash address: 0x000f8000
    [00:00:00.252,899] <inf> main: Read data
                                10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f |........ ........
    [00:00:00.252,929] <inf> main: Erasing flash page
    [00:00:00.337,463] <inf> main: Read data
                                ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff |........ ........
    [00:00:00.337,493] <inf> main: Write data to flash
    [00:00:00.337,677] <inf> main: Waiting for write to complete
    [00:00:00.337,677] <inf> main: Read data
                                10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f |........ ........
    ```

* 操作地址
    为什么操作的地址`#define FLASH_ADDRESS 0x000f8000`，因为在`nrf52840dk_nrf52840.dts`中的`flash`的`storage_partition`保留给了`nvm`使用;虽然我们这个demo没有走设备树，但是我还是认为这一片flash是可以使用的。
    ```c
        &flash0 {

        partitions {
            compatible = "fixed-partitions";
            #address-cells = <1>;
            #size-cells = <1>;

            boot_partition: partition@0 {
                label = "mcuboot";
                reg = <0x00000000 0x0000C000>;
            };
            slot0_partition: partition@c000 {
                label = "image-0";
                reg = <0x0000C000 0x00076000>;
            };
            slot1_partition: partition@82000 {
                label = "image-1";
                reg = <0x00082000 0x00076000>;
            };

            /*
            * The flash starting at 0x000f8000 and ending at
            * 0x000fffff is reserved for use by the application.
            */

            /*
            * Storage partition will be used by FCB/LittleFS/NVS
            * if enabled.
            */
            storage_partition: partition@f8000 {
                label = "storage";
                reg = <0x000f8000 0x00008000>;
            };
        };
    };
    ```


  
