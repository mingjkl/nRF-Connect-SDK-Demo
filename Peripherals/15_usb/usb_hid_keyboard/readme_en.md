## Introduction
This is a demo of implementing a USB-HID keyboard. It mainly achieves two functions: sending key reports from the device to the host by triggering physical keys, and changing the LED state on the device when the host sends the state of the CAPS LOCK key.

## Key Code
* Add the necessary `kconfig` configuration in `prj.conf`
    ```c
    CONFIG_USB_DEVICE_STACK=y
    CONFIG_USB_DEVICE_HID=y
    CONFIG_USB_DEVICE_PRODUCT="Zephyr HID Keyboard sample"
    CONFIG_USB_DEVICE_PID=0x0007
    CONFIG_USB_DEVICE_INITIALIZE_AT_BOOT=n
    ```
    This configuration includes information such as the device name and PID.

* Add the necessary header files in `main.c`
    ```c
    #include <zephyr/device.h>
    #include <zephyr/usb/usb_device.h>
    #include <zephyr/usb/class/usb_hid.h>
    #include <hal/nrf_gpio.h>
    ```
    The `hal/nrf_gpio.h` is used for configuring the LED and button pins, but we won't explain the LED and button configuration here.

* Define a device structure
    ```c
    const struct device *hid_dev;
    ```
    
* Bind the HID device to the device structure
    ```c
    hid_dev = device_get_binding("HID_0");
    ```

* Define a HID report descriptor
    ```c
    static const uint8_t hid_report_desc[] = HID_KEYBOARD_REPORT_DESC();
    ```
    Here we use the `HID_KEYBOARD_REPORT_DESC` macro provided by Zephyr to generate the HID keyboard report descriptor.

* Bind the HID report descriptor to our HID device
    ```c
    usb_hid_register_device(hid_dev,
                            hid_report_desc, sizeof(hid_report_desc),
                            &ops);
    ```
* By the way, we need to add `ops`, which is the operation callback functions for the HID device
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
    In our demo, only the `set_report` callback function is implemented. This function is mainly used for the host to send the state of the CAPS LOCK key to the device. Upon receiving the state of the CAPS LOCK key, the device controls the LED state based on it. We won't go into detail about other callback functions here.

* Initialize the USB HID device
    ```c
    usb_hid_init(hid_dev);
    ```

* Enable the USB device
    ```c
    static void status_cb(enum usb_dc_status_code status, const uint8_t *param)
    {
        usb_status = status;
    }

    // .....

    usb_enable(status_cb);
    ```
    Here we define a `status_cb` callback function to get the status of the USB device, and then enable the USB device in the `usb_enable` function.

* At this point, our USB HID initialization is complete. We compile and flash the program, and we can see a new HID keyboard device on our host, which is our PC. When we press the CAPS LOCK key on our keyboard, our PC sends the state of the CAPS LOCK key to our development board, which is the device side. Upon receiving the state of the CAPS LOCK key, the device controls the LED state based on it.

* Next, we implement sending key reports from the device to the host. We initialize a button pin and blockingly monitor the status of the button pin in the main loop. When the button is pressed, we send a report to the host.
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
    Here we define a `report` array to store the report we want to send. In this case, we send the report of the digit `5`. It's worth noting that normally the keyboard doesn't continuously send reports, but only sends reports when a key is pressed or released. However, in our demo, we continuously send reports in the main loop for debugging convenience.

* Compile and flash the program. Then, when we press the button on our development board, our host, which is our PC, will receive the input of the digit `5`.