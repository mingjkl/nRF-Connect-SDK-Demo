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

uint8_t rx_data[CONFIG_ESB_MAX_PAYLOAD_LENGTH];
uint8_t tx_data[CONFIG_ESB_MAX_PAYLOAD_LENGTH];
uint8_t rx_len;
uint8_t tx_len;

bool eesb_ready = true;

void eesb_rx_mode_event_handle(struct esb_evt const *event)
{

	int pipe = -1;

	switch (event->evt_id) {
	case ESB_EVENT_TX_SUCCESS:
		break;
	case ESB_EVENT_TX_FAILED:
		break;
	case ESB_EVENT_RX_RECEIVED:
		while(pipe < 0)
		{
			pipe = eesb_rx_data_read(rx_data, rx_len);
		}

		tx_data[0] = pipe;
		eesb_write_payload_pipe(pipe, tx_data, tx_len, false);

		break;
	}
}

int main(void)
{
	printk("running eesb_init\n");

	struct eesb_addr_config addr = EESB_DEFAULT_ADDR;

	esb_set_rf_channel(32);
	eesb_rx_mode_init(ESB_BITRATE_2MBPS, ESB_TX_POWER_0DBM, &addr, eesb_rx_mode_event_handle);
	eesb_set_enable_pipes(7);
	
	return 0;
}
