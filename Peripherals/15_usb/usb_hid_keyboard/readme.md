## 简介
这是一个实现`USB-HID`键盘的demo，主要实现了两个功能，一个是通过触发物理按键，实现`device`端向`host`端发送按键的`report`，另一个是`host`回传`CAPS LOCK`键的状态时，`device`端的`LED`灯状态也会发生变化。

## 关键代码
* 在`prj.conf`中加入必要的`kconfig`配置
    ```c
    CONFIG_USB_DEVICE_STACK=y
    CONFIG_USB_DEVICE_HID=y
    CONFIG_USB_DEVICE_PRODUCT="Zephyr HID Keyboard sample"
    CONFIG_USB_DEVICE_PID=0x0007
    CONFIG_USB_DEVICE_INITIALIZE_AT_BOOT=n
    ```
    这里面包含了包含了设备名称、PID等信息。
* 在`main.c`中加入必要的头文件
    ```c
    #include <zephyr/device.h>
    #include <zephyr/usb/usb_device.h>
    #include <zephyr/usb/class/usb_hid.h>
    #include <hal/nrf_gpio.h>
    ```
    这里面的`hal/nrf_gpio.h`是在`LED`和按键引脚配置时使用的，不过在后面我们不解释`LED`和按键的配置了。
* 定义一个设备结构体
    ```c
    const struct device *hid_dev;
    ```
    
* 给设备结构体`binding`一个`HID`设备
    ```c
    hid_dev = device_get_binding("HID_0");
    ```
* 定义一个`HID`报告描述符
    ```c
    static const uint8_t hid_report_desc[] = HID_KEYBOARD_REPORT_DESC();
    ```
    这里我们使用了`HID_KEYBOARD_REPORT_DESC`宏，这个宏是`Zephyr`提供的，用于生成`HID`键盘的报告描述符。
* 给我们的设备`HID`设备绑定一个`HID`报告描述符
    ```c
    usb_hid_register_device(hid_dev,
				hid_report_desc, sizeof(hid_report_desc),
				&ops);
    ```
* 对了，我们需要补充一下`ops`，这个是`HID`设备的操作回调函数
    ```c

    static int hid_on_set_report_cb(const struct device *dev,
			 struct usb_setup_packet *setup, int32_t *len,
			 uint8_t **data)
    {
        if(*len != 1) {
            return -EINVAL;
        }

        **data & HID_KBD_LED_CAPS_LOCK ? led_set(true) : led_set(false);

        return 0;
    }

    //......

    static const struct hid_ops ops = {
    .set_report = hid_set_report,
    };

    ```
    在我们的demo中只实现了`set_report`这个回调函数，这个函数主要是用于`host`端向`device`端发送`CAPS LOCK`键的状态，`device`端接收到`CAPS LOCK`键的状态后，会根据`CAPS LOCK`键的状态来控制`LED`灯的状态。其他的回调函数我们没有实现，所以这里就不展开了。

* 进行`usb hid`设备的初始化
    ```c
    usb_hid_init(hid_dev);
    ```

* 使能`usb`设备
    ```c
    static void status_cb(enum usb_dc_status_code status, const uint8_t *param)
    {
        usb_status = status;
    }

    // .....

    usb_enable(status_cb);
    ```
    这里我们定义了一个`status_cb`回调函数，用于获取`usb`设备的状态，然后在`usb_enable`函数中使能`usb`设备。

* 到这里我们的`usb hid`已经初始化完成了，我们编译并烧录程序，可以在我们的`host`端，也就是我们的PC上可以看到多了一个`HID`的键盘设备；当我们按下我们正在敲代码的键盘上的`CAPS LOCK`键时,我们的PC会向我们开发板，也就是`device`端发送`CAPS LOCK`键的状态，`device`端接收到`CAPS LOCK`键的状态后，会根据`CAPS LOCK`键的状态来控制`LED`灯的状态。

* 接下来，我们实现`device`端向`host`端发送按键的`report`。我们初始化一个按键引脚，在主循环中阻塞式的监控按键引脚的状态，当按键被按下时发送一个`report`给`host`端。
    ```c
    void main(void)
    {
        //......

        while(1)
        {
            if(!nrf_gpio_pin_read(BTN_PIN))
            {
                report[2] = HID_KEY_5;
                ret = hid_int_ep_write(hid_dev, report, sizeof(report), NULL);
            }
            else
            {
                report[2] = 0;
                ret = hid_int_ep_write(hid_dev, report, sizeof(report), NULL);
            }
            k_msleep(10);
        }
    }
    ```
    这里我们定义了一个`report`数组，用于存放我们要发送的`report`，我们这里发送的是数字`5`的`report`;另外需要注意的一点是，正常情况下，键盘不会一直发送`report`，而是在按键被按下或者松开时发送`report`;不过在我们的demo中，我们为了方便调试，所以我们在主循环中一直发送`report`。

* 编译并烧录程序，然后按下我们开发板上的按键，我们的`host`端，也就是我们的PC上，可以看到数字`5`被输入到了我们的PC上。