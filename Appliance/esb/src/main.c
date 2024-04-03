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

int main(void)
{
	printk("running eesb_init\n");

	struct eesb_addr_config addr = EESB_DEFAULT_ADDR;
	
	enum esb_mode mode = ESB_MODE_PRX;

	if(mode == ESB_MODE_PTX)
	{
		eesb_tx_mode_init(ESB_BITRATE_2MBPS, ESB_TX_POWER_0DBM, &addr);
		eesb_tx_signal_test(66);
	}
	else
	{
		esb_set_rf_channel(66);
		eesb_rx_mode_init(ESB_BITRATE_2MBPS, ESB_TX_POWER_0DBM, &addr);
		
	}

	return 0;
}
