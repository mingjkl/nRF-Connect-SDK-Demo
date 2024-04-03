<!--
 * @Author: emmovo
 * @Date: 2023-12-05 08:57:03
 * @LastEditors: emmovo mingjkl@live.com
 * @LastEditTime: 2023-12-05 11:34:21
 * @FilePath: \0_project\README.md
 * @Description: 
 * 
 * Copyright (c) 2023 by mingjkl@live.com/emmovo.com, All Rights Reserved. 
-->

## Project Directory

* CMakeLists.txt
  * Project build and configuration
* prj.conf
  * Custom Kconfig configuration for the project
* .overlay
  * Custom device tree content overlay for the project
* xxx.c
* xxx.h

## Project Build
### Kconfig and DTS:

  Kconfig and DTS are important components of the project build:
  * Kconfig is used to configure various options of the kernel, enabling or disabling specific features such as peripheral instances.
    * It allows quick enabling or disabling of specific functional modules, drivers, or options.
    * Configuration information is separated from the actual implementation code, facilitating portability between hardware platforms and systems.
    * It improves code readability and maintainability.
    * Similar to 'sdk_config.h' in nRF5 SDK.
  * DTS is a hierarchical device description file that includes device properties, drivers, memory configurations, etc.
    * Hardware configuration information is separated from the driver programs, achieving decoupling of hardware and software.
    * Hardware is abstracted into a unified access method, relieving software developers from the need to deal with low-level hardware details.

  During the project build process, a '.config' file and 'zephyr.dts' file will be generated in the 'build/zephyr' directory of the project.

  * The '.config' file consists of the following file contents:
    * zephyr/arch/Kconfig
    * zephyr/drivers/xxx/Kconfig
    * xxx_board/Kconfig
    * project/prj.conf (highest priority, overrides previous contents)
  * The 'zephyr.dts' file consists of the following file contents:
    * xxx_board/xxx.dts
    * xxx_board/dtsi
    * project/.overlay (highest priority, overrides previous contents)
  
  The relationship is shown in the following diagram: ![1701743890427](image/README/1701743890427.png)
  
### CMakeLists.txt
  * CMakeLists.txt is the configuration file for CMake, used to describe project build rules and processes.
  * It generally includes the following contents:
    * Required minimum version of CMake
    * Project name and version
    * Source files
    * Header files
    * ...

## Writing Guidelines
### Kconfig
  * Refer to relevant demos
    * sdk/zephyr/samples
    * sdk/nrf/samples
  * [Kconfig search](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/kconfig/index.html)
  * Check the configurations in menuconfig
### DTS
  * Refer to sdk/zephyr/boards
  * [Nordic Semiconductor Bindings index](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/build/dts/api/bindings.html#dt-vendor-nordic)
  * [Device Tree Guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/build/dts/index.html)
### CMakeLists.txt
  * [Writing CMakeLists Files](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Writing%20CMakeLists%20Files.html)
  * [emmovo/nRF connect sdk demo/A1_include](https://gitee.com/mingkjl/nordic-connect-sdk-demo/tree/master/A1_include)
