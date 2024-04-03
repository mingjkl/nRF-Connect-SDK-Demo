/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(uart, LOG_LEVEL_DBG);

#define RECEIVE_TIMEOUT_US 1000 
#define SLEEP_TIME_MS   100
#define RECEIVE_BUFF_SIZE 255
#define SEND_BUFF_SIZE 255

const struct device *uart= DEVICE_DT_GET(DT_NODELABEL(uart0));
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};

uint8_t tx_buf[SEND_BUFF_SIZE];
uint8_t tx_buf_len = 0;

int uart_data_send(const struct device *dev, const uint8_t *buf, uint8_t len)
{
	int ret;

	ret = uart_tx(dev, buf, len, SYS_FOREVER_MS);
	if (ret) {
		LOG_ERR("Failed to send data over UART");
		return ret;
	}

	return 0;
}

void work_uart_send(struct k_work *work)
{
	if (tx_buf_len > 0)
	{
		uart_data_send(uart, tx_buf, tx_buf_len);
		tx_buf_len = 0;
	}
}

K_WORK_DEFINE(work_uart_send_data, work_uart_send);


static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
switch (evt->type) {

	case UART_RX_RDY:
		LOG_HEXDUMP_DBG(evt->data.rx.buf, evt->data.rx.len, "data: ");

		for(uint16_t i = 0; i < evt->data.rx.len; i++){
			tx_buf[i] = evt->data.rx.buf[i];
		}

		tx_buf_len = evt->data.rx.len;

		k_work_submit(&work_uart_send_data);

		uart_rx_disable(dev);

		break;
	case UART_RX_DISABLED:
		uart_rx_enable(dev ,rx_buf,sizeof rx_buf,RECEIVE_TIMEOUT_US);
		break;
	default:
		break;
	}
}

int main(void)
{
	int ret;

	if (!device_is_ready(uart)){
		LOG_ERR("UART device not ready\r\n");
		return 1 ;
	}
	else{
		LOG_INF("UART device ready\r\n");
	}

	ret = uart_callback_set(uart, uart_cb, NULL);
	if (ret) {
		LOG_ERR("Cannot set callback");
		return 1;
	}

	ret = uart_rx_enable(uart, rx_buf, sizeof rx_buf, RECEIVE_TIMEOUT_US);
	if (ret) {
		LOG_ERR("Cannot enable rx");
		return 1;
	}


	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}

	return 0;
}
