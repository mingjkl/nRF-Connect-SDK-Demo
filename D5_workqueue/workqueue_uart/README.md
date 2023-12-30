## 简介
这个demo是将uart的数据echo改为通过workqueue实现的uart数据echo。以降低中断的响应时间，提高系统的实时性。

这个demo是基于[\3_uart\uart_zephyr]()的集成上实现的，将原本的demo上在`main`函数的`while(1)`中实现的uart数据echo改成了，通过workqueue实现的uart数据echo。

## 代码分析
* 删除main部分代码
  ```
    int main(void)
    {
        // ...

        while (1) {

            if(tx_buf_len > 0){
                uart_data_send(uart, tx_buf, tx_buf_len);
                tx_buf_len = 0;
            }

            // ...
        }

    }
  ```
* 建立一个work
  ```
  K_WORK_DEFINE(work_uart_send_data, work_uart_send);
  ```
* 补充work的实现
  ```
    void work_uart_send(struct k_work *work)
    {
        if (tx_buf_len > 0)
        {
            uart_data_send(uart, tx_buf, tx_buf_len);
            tx_buf_len = 0;
        }
    }
  ```
* 在uart的中断函数中，将work提交到workqueue中
  ```
    void uart_irq_callback(struct device *dev)
    {
        // ...
        k_work_submit(&work_uart_send_data);
    }
  ```
* 编译并烧录程序
* 验证
    * 我们打开串口，实现uart的数据echo。这里我们的echo就改为了workqueue实现的echo。
    * 不过我们这里使用的是默认的workqueue，如果我们需要自定义优先级和stack的大小，可以自定义workqueue，具体可以参考demo。
