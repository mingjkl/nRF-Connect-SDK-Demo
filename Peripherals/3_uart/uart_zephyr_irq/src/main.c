/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(uart, LOG_LEVEL_DBG);

#define UART_BUF_SIZE 256			// uart rx buffer size
static struct uart_rx_buf
{
	uint8_t buffer[UART_BUF_SIZE];
	uint8_t pointer;
};

static struct uart_rx_buf rx_buf = {
	.pointer = 0,
};


static void uart_rx_handler(struct device *dev)
{

	uint8_t data;

    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) 
    {

        uart_fifo_read(dev, &data, sizeof(data));

		rx_buf.buffer[rx_buf.pointer] = data;
		rx_buf.pointer++;

		if((rx_buf.pointer > UART_BUF_SIZE) || (data == '\n'))
		{
			LOG_HEXDUMP_DBG(rx_buf.buffer, rx_buf.pointer, "RX Data: ");
			rx_buf.pointer = 0;
			memset(rx_buf.buffer, 0, sizeof(rx_buf.buffer));
			return;
		}
    }
}

int uart_init(void)
{
	const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
	if(!uart_dev)
	{
		LOG_ERR("Cannot find uart\n");
		return -1;
	}

	uart_irq_callback_set(uart_dev, uart_rx_handler);
	uart_irq_rx_enable(uart_dev);

	LOG_INF("UART initialized\n");

	return 0;
}

int uart_send(uint8_t *data, uint16_t len)
{
	const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
	if(!uart_dev)
	{
		LOG_ERR("Cannot find uart\n");
		return -1;
	}

	for(uint16_t i = 0; i < len; i++)
	{
		uart_poll_out(uart_dev, data[i]);
	}
	return 0;
}

int main(void)
{
	printf("This is UART Demo! %s\n", CONFIG_BOARD);
	uart_init();
	uart_send("UART send data test\n", sizeof("UART send data test\r\n"));
	return 0;
}
