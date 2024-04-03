## 简介
这是一个使用`zephyr/drivers/i2c.h`驱动的I2C设备的demo。在demo是一个通过`i2c`对`AT24C02`读写，演示`zephyr`下`i2c`的使用。

## 关键代码
* 在`prj.conf`中配置`I2C`
    ```c
        CONFIG_LOG=y
        CONFIG_I2C=y
    ```

* 新建`.overlay`文件，并加入`I2C`的配置
    ```c
    &spi1 {
    status = "disabled";
    };

    &i2c0 {
        status = "disabled";
    };

    &i2c1 {
        status = "okay";
        clock-frequency = <400000>;
    };

    &i2c1_default {
        group1 {
            psels = <NRF_PSEL(TWIM_SDA, 0, 31)>, <NRF_PSEL(TWIM_SCL, 0, 30)>;
        };
    };
    ```
    这里为了避免`nrf52840dk`上的`spi1`和`i2c0`冲突，将`spi1`和`i2c0`禁用，`i2c1`使能，并设置时钟频率为`400000`，并配置`i2c1`的的引脚`psels`，`TWIM_SDA`和`TWIM_SCL`分别对应`nrf52840dk`上的`P0.31`和`P0.30`。

* 在`main.c`中加入必要的头文件
    ```c
        #include <zephyr/device.h>
        #include <zephyr/devicetree.h>
        #include <zephyr/logging/log.h>
        #include <zephyr/drivers/i2c.h>
    ```

* 定义一个`i2c`设备
    ```c
        const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));
    ```

* 使用前问问`i2c`设备是否可用
    ```c
        if(!device_is_ready(i2c_dev)){
            LOG_ERR("I2C device is not ready");
            return -1;
        }
        else{
            LOG_INF("I2C device is ready");
        }
    ```

* `i2c`的读写我们使用`i2c_transfer`函数
  * 包含三个参数
    * `i2c_dev`：`i2c`设备
    * `msg`：`i2c_msg`结构体数组，用于存放读写数据
    * `num_msgs`：`msg`的个数
    * `addr`：设备地址, 7位地址，不包括读写标志
  * `i2c_msg`结构体
    * `buf`：数据缓冲区，指向一个数组
    * `len`：数据长度
    * `flags`：读写标志
  * 控制读写的`flags`
    * `I2C_MSG_WRITE`：写标志
    * `I2C_MSG_READ`：读标志
    * `I2C_MSG_RESTART`：重启标志
    * `I2C_MSG_STOP`：停止标                                                                               志

* 读写`AT24C02`的数据
    * 写数据
      * 先写入设备地址，再写内存地址，最后写入数据；
      * 设备地址作为参数给到`i2c_transfer`函数
      * 内存地址和数据分别放入`i2c_msg`结构体中；这里面我们可以使用一个`i2c_msg`结构体数组，内存地址写在`i2c_msg.buf[0]`中，数据写在`i2c_msg.buf[1]`及之后的位置；也可以使用两个`i2c_msg`结构体，一个用于写内存地址，一个用于写数据；
      * 注意最后一个`i2c_msg`结构体的`flags`要加上`I2C_MSG_STOP`标志；
    * 读数据
      * 先写入设备地址，再写内存地址，再写入设备地址，最后读取数据；
      * 设备地址作为参数给到`i2c_transfer`函数
      * 使用两个`i2c_msg`结构体，一个用于写内存地址，一个用于读数据；
      * 写内存地址的`i2c_msg`结构体的`flags`为`I2C_MSG_WRITE | I2C_MSG_STOP`;读数据的`i2c_msg`结构体的`flags`为`I2C_MSG_READ | I2C_MSG_STOP`；

* 具体`eeprom`的读写我们看详细代码
* 编译和烧录代码，我们通过`log`可以看到数据读写是否成功，同时我们也可以通过逻辑分析仪观察`i2c`的数据的读写情况。下面是我们的逻辑分析仪的截图：
  * 写过程
    ![write](img/w.PNG)

  * 读过程
    ![read](img/r.PNG)