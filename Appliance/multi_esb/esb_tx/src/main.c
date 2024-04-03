/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "eesb.h"
#include "esb.h"
#include "zephyr/sys/printk.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define RF_TEST_CHANNEL 32	// 2400MHz + 32MHz = 2432MHz

uint8_t rx_data[CONFIG_ESB_MAX_PAYLOAD_LENGTH];
uint8_t rx_len;
bool eesb_ready = true;


static void eesb_tx_mode_event_handle(struct esb_evt const *event)
{
	
	int pipe = -1;
	eesb_ready = true;

	switch (event->evt_id) {
	case ESB_EVENT_TX_SUCCESS:
		break;
	case ESB_EVENT_TX_FAILED:
		break;
	case ESB_EVENT_RX_RECEIVED:
		while (pipe < 0) 
		{
			pipe = eesb_rx_data_read(rx_data, rx_len);
		}
		LOG_DBG("[%d] %d", pipe, rx_data[0]);	// log the pipe and the first byte of the received data
		break;
	}
}

void eesb_tx_pipe_test(void)
{
	uint8_t pipe = 0;

	esb_set_rf_channel(RF_TEST_CHANNEL);

	while(1)
	{
		if(eesb_ready)
		{
			eesb_ready = false;
			
			uint8_t data[8] = {0x01, 0x00, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

			data[0] = pipe;
			eesb_write_payload_pipe(pipe, data, sizeof(data), true);

			pipe++;
			if(pipe > 7) pipe = 0;

		}
		k_msleep(1);
	}

}

int main(void)
{
	printk("running eesb_init\n");

	struct eesb_addr_config addr = EESB_DEFAULT_ADDR;
	
	eesb_tx_mode_init(ESB_BITRATE_2MBPS, ESB_TX_POWER_0DBM, &addr, eesb_tx_mode_event_handle);
	eesb_tx_pipe_test();


	return 0;
}
