## 简介
这是一个关于ring buffer的使用demo

## 关键内容
* 需要在prj.conf中添加如下配置
    ```
    CONFIG_RING_BUFFER=y
    ```
* 需要在main.c中添加如下代码
    ```
    #include <zephyr/sys/ring_buffer.h>
    ```

## 关键API
* 声明ring buffer
    ```
    RING_BUF_DECLARE(my_ring_buf, RING_BUF_SIZE);
    ```
  其中RING_BUF_SIZE为ring buffer的大小，单位为字节
* 判断ring buffer 还有多少空间
    ```
    ret = ring_buf_space_get(&my_ring_buf);
    ```
    返回的ret为ring buffer的剩余空间，单位为字节
* 写入ring buffer
    ```
    ret = ring_buf_put(&my_ring_buf, data, cnt);
    ```
    其中data为写入的数组，cnt为写入的数组长度，ret为实际写入的数组长度
* 读取ring buffer
    ```
    ret = ring_buf_get(&my_ring_buf, data, cnt);
    ```
    其中data为读取的数组，cnt为读取的数组长度，ret为实际读取的数组长度

## 注意事项
* 当ring buffer的空间不足时，写入ring buffer会失败，返回值为实际写入的长度；
* 当ring buffer的数据不足时，读取ring buffer会失败，返回值为实际读取的长度；