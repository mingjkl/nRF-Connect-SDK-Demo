## Introduction
This demo modifies the UART data echo to be implemented through a workqueue instead of directly in the interrupt handler. This approach aims to reduce interrupt response time and enhance the system's real-time performance.

The demo is based on the integration of [\3_uart\uart_zephyr](3_uart\uart_zephyr) and modifies the original demo by replacing the UART data echo implementation in the `main` function's `while(1)` loop with a workqueue-based approach.

## Code Analysis
* Remove part of the code in `main`:
  ```c
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
* Establish a work item:
  ```c
  K_WORK_DEFINE(work_uart_send_data, work_uart_send);
  ```
* Implement the work function:
  ```c
    void work_uart_send(struct k_work *work)
    {
        if (tx_buf_len > 0)
        {
            uart_data_send(uart, tx_buf, tx_buf_len);
            tx_buf_len = 0;
        }
    }
  ```
* In the UART interrupt function, submit the work to the workqueue:
  ```c
    void uart_irq_callback(struct device *dev)
    {
        // ...
        k_work_submit(&work_uart_send_data);
    }
  ```
* Compile and flash the program.
* Verification:
    * Open the serial port to realize UART data echo. Here, our echo is now implemented using a workqueue.
    * However, we are using the default workqueue. If you need to customize the priority and stack size, you can define your own workqueue. For details, refer to the [workqueue](D5_workqueue\workqueue).