## Introduction
This is a demo on the use of a ring buffer.

## Key Content
* The following configuration needs to be added in `prj.conf`:
    ```
    CONFIG_RING_BUFFER=y
    ```
* The following code needs to be added in `main.c`:
    ```
    #include <zephyr/sys/ring_buffer.h>
    ```

## Key APIs
* Declaring a ring buffer:
    ```
    RING_BUF_DECLARE(my_ring_buf, RING_BUF_SIZE);
    ```
  Here, `RING_BUF_SIZE` is the size of the ring buffer in bytes.
* Checking how much space is left in the ring buffer:
    ```
    ret = ring_buf_space_get(&my_ring_buf);
    ```
    The returned `ret` is the remaining space in the ring buffer, in bytes.
* Writing to the ring buffer:
    ```
    ret = ring_buf_put(&my_ring_buf, data, cnt);
    ```
    Here, `data` is the array to be written, `cnt` is the length of the array to be written, and `ret` is the actual length of the array written.
* Reading from the ring buffer:
    ```
    ret = ring_buf_get(&my_ring_buf, data, cnt);
    ```
    Here, `data` is the array to be read, `cnt` is the length of the array to be read, and `ret` is the actual length of the array read.

## Precautions
* When there is insufficient space in the ring buffer, writing to the ring buffer will fail, and the return value will be the actual length of the array written;
* When there is insufficient data in the ring buffer, reading from the ring buffer will fail, and the return value will be the actual length of the array read.