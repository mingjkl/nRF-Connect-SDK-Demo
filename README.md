
![Helloworld](img/helloworld.png)

# nRF Connect SDK Demo

## ✨ 介绍

这个项目是基于`nRF Connect SDK`做的一个demo合集，里面包含了很多demo，这些demo希望可以帮助大家快速上手`nRF Connect SDK`。同时可以用最少的代码量实现一些功能。demo参考的源有些多，有`NRF5 SDK`的demo，`Zephyr`的sample和`nrfx`的sample；内容主要是围绕片上外设的驱动为主，也包含了一些开发工具的介绍。如果有不正确的地方，欢迎指正。

注意：`NCS`更新比较频繁，demo不能做到及时更新，所以demo中的代码可能会有些过时，但是还是希望可以给大家带来参考；

## 🖥 开发环境
* 开发平台：Windows 11
* 开发工具：Vscode + nRF Connect
* SDK版本：Nordic connect sdk v2.4.2
* 工具链版本：Nordic connect sdk Toolchain v2.5.0
* 开发板：nRF52840 DK

## 📢 完成度
* ### 外设类
  * [x] GPIO
  * [x] UART
  * [x] ADC
  * [x] SPI
  * [x] I2C
  * [x] PPI
  * [x] TIMER
  * [x] PWM
  * [ ] RAIDO
  * [ ] USBD
    * [x] USB-HID Keyboard
    * [x] USB-HID Transparent
    * [ ] USB-Audio
  * [x] WDT
  * [x] RTC
  * [x] I2S
  * [x] NVMS

* ### 系统类
  * [x] Thread Create
  * [x] Thread Scheduling
  * [x] Thread Semaphore
  * [x] FIFO
  * [x] Ringbuffer
  * [x] Workqueue

* ### 工具类
  * [x] RTT
  * [x] CMake
  * [x] JScope
  * [x] SystemView   
  * [x] Logging 

* ### 文件目录
    ```
    ├─ Appliance    // 一些应用的demo
    ├─ Peripheral   // 外设应用的demo
    ├─ Tools        // 工具类的demo
    └─ Zephyr       // Zephyr系统类的demo
    ```