## Introduction
This is a simple example routine that utilizes the `nrfx_nvmc.h` library to perform internal flash read and write operations on the nRF52840. The example first reads data from a specified address, checks if the data at that address is `0xFF`, and if so, writes new data. If the data is not `0xFF`, the page is erased before writing the new data. After completing the read and write operations, the data is read again and printed.

## Key Code Snippets
* Add necessary configurations in `prj.conf`
    ```c
    CONFIG_NRFX_NVMC=y
    CONFIG_FLASH=y
    ```

* Include necessary header file
    ```c
    #include <nrfx_nvmc.h>
    ```

* Get page size
    ```c
    uint32_t page_size = nrfx_nvmc_flash_page_size_get();
    ```

* Page erase
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
    Ensure that the page address is a multiple of the page size before calling `nrfx_nvmc_page_erase()`. After erasing the page, wait for the erase operation to complete to avoid errors.

* Write data
    ```c
    nrfx_nvmc_bytes_write(wr_address, data_to_write, sizeof(data_to_write));
    while(nrfx_nvmc_write_done_check() == false)
    {
        // wait for write to complete
    }
    ```
    Call `nrfx_nvmc_bytes_write()` with the address and data to write. After writing, wait for the write operation to complete. The function writes byte data; consider using `nrfx_nvmc_words_write()` for writing `uint32_t` data.

* Read data
    ```c
    uint8_t *data_to_read = (uint8_t *)wr_address;

    LOG_INF("Data read from flash address: 0x%08x", (uint32_t)data_to_read);
    LOG_HEXDUMP_INF(data_to_read, 16, "Read data");
    ```
    Directly read the address to retrieve the data.

* LOG content
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

* Operation Address
    The reason for operating at address `#define FLASH_ADDRESS 0x000f8000` is because in `nrf52840dk_nrf52840.dts`, the `storage_partition` of the `flash` is reserved for use by `nvm`. Although our demo does not utilize the device tree, I still believe that this flash region is available for use.
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