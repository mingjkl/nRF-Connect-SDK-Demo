#include "eesb.h"
#include "esb.h"
#include <zephyr/drivers/clock_control/nrf_clock_control.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(eesb, LOG_LEVEL_DBG);

static bool ready = true;
static struct esb_payload rx_payload;
static struct esb_payload tx_payload = ESB_CREATE_PAYLOAD(0,
	0x01, 0x00, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);

static int clocks_start(void)
{
	int err;
	int res;
	struct onoff_manager *clk_mgr;
	struct onoff_client clk_cli;

	clk_mgr = z_nrf_clock_control_get_onoff(CLOCK_CONTROL_NRF_SUBSYS_HF);
	if (!clk_mgr) {
		LOG_ERR("Unable to get the Clock manager");
		return -ENXIO;
	}

	sys_notify_init_spinwait(&clk_cli.notify);

	err = onoff_request(clk_mgr, &clk_cli);
	if (err < 0) {
		LOG_ERR("Clock request failed: %d", err);
		return err;
	}

	do {
		err = sys_notify_fetch_result(&clk_cli.notify, &res);
		if (!err && res) {
			LOG_ERR("Clock could not be started: %d", res);
			return res;
		}
	} while (err);

	LOG_DBG("HF clock started");
	return 0;
}

static void eesb_event_handler(struct esb_evt const *event)
{
	ready = true;

	switch (event->evt_id) {
	case ESB_EVENT_TX_SUCCESS:
		LOG_DBG("TX SUCCESS EVENT");
		break;
	case ESB_EVENT_TX_FAILED:
		LOG_DBG("TX FAILED EVENT");
		break;
	case ESB_EVENT_RX_RECEIVED:
		while (esb_read_rx_payload(&rx_payload) == 0) {
			LOG_DBG("Packet received, len %d : "
				"0x%02x, 0x%02x, 0x%02x, 0x%02x, "
				"0x%02x, 0x%02x, 0x%02x, 0x%02x",
				rx_payload.length, rx_payload.data[0],
				rx_payload.data[1], rx_payload.data[2],
				rx_payload.data[3], rx_payload.data[4],
				rx_payload.data[5], rx_payload.data[6],
				rx_payload.data[7]);
		}
		break;
	}
}

void eesb_tx_signal_test(uint8_t channel)
{
    int err;

    esb_set_rf_channel(channel);

    while(1)
    {
        if(ready)
        {
            ready = false;
            esb_flush_tx();
            err = esb_write_payload(&tx_payload);
			if (err) {
				LOG_ERR("Payload write failed, err %d", err);
			}
        }
        k_msleep(1);
    }
}

void eesb_init(struct esb_config *config, struct eesb_addr_config *addr_config)
{
    int err;

    err = clocks_start();
    if (err) {
        LOG_ERR("Clocks start failed: %d", err);
        return;
    }

	config->event_handler = eesb_event_handler;

    err = esb_init(config);
    if (err) {
        LOG_ERR("ESB initialization failed: %d", err);
        return;
    }

	err = esb_set_base_address_0(addr_config->base_addr_0);
	if (err) {
		LOG_ERR("Base address 0 set failed: %d", err);
		return;
	}
	esb_set_base_address_1(addr_config->base_addr_1);
	if (err) {
		LOG_ERR("Base address 1 set failed: %d", err);
		return;
	}
	esb_set_prefixes(addr_config->addr_prefix, ARRAY_SIZE(addr_config->addr_prefix));
	if (err) {
		LOG_ERR("Address prefixes set failed: %d", err);
		return;	
	}

    LOG_DBG("EESB initialized");
	k_msleep(1000);
}


void eesb_tx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config)
{
	struct esb_config config = ESB_DEFAULT_CONFIG;

	config.protocol = ESB_PROTOCOL_ESB_DPL;
	config.mode = ESB_MODE_PTX;
	config.event_handler = eesb_event_handler;
	config.bitrate = bitrate;
	config.crc = ESB_CRC_16BIT;
	config.tx_output_power = output_power;
	config.retransmit_delay = CONFIG_EESB_RETRANSMIT_DELAY;
	config.retransmit_count = CONFIG_EESB_RETRANSMIT_COUNT;
	config.tx_mode = ESB_TXMODE_AUTO;
	config.payload_length = CONFIG_EESB_PAYLOAD_LENGTH;
	config.selective_auto_ack = false;
	
	#ifdef CONFIG_EESB_FAST_RAMP_UP
	config.use_fast_ramp_up = true;
	#else
	config.use_fast_ramp_up = false;
	#endif

	eesb_init(&config, addr_config);
}

void eesb_rx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config)
{
	struct esb_config config = ESB_DEFAULT_CONFIG;

	config.protocol = ESB_PROTOCOL_ESB_DPL;
	config.mode = ESB_MODE_PRX;
	config.event_handler = eesb_event_handler;
	config.bitrate = bitrate;
	config.crc = ESB_CRC_16BIT;
	config.tx_output_power = output_power;
	config.retransmit_delay = CONFIG_EESB_RETRANSMIT_DELAY;
	config.retransmit_count = CONFIG_EESB_RETRANSMIT_COUNT;
	config.tx_mode = ESB_TXMODE_AUTO;
	config.payload_length = CONFIG_EESB_PAYLOAD_LENGTH;
	config.selective_auto_ack = false;
	
	#ifdef CONFIG_EESB_FAST_RAMP_UP
	config.use_fast_ramp_up = true;
	#else
	config.use_fast_ramp_up = false;
	#endif

	eesb_init(&config, addr_config);
	esb_start_rx();
}
