#ifndef __EESB_H__
#define __EESB_H__

#include "esb.h"

#define EESB_DEFAULT_ADDR                                           \
{                                                                   \
    .base_addr_0 = {0xE7, 0xE7, 0xE7, 0xE7},                        \
    .base_addr_1 = {0xC2, 0xC2, 0xC2, 0xC2},                        \
    .addr_prefix = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88} \
}

struct eesb_addr_config {
    uint8_t base_addr_0[4];
    uint8_t base_addr_1[4];
    uint8_t addr_prefix[8];
};

void eesb_init(struct esb_config *config, struct eesb_addr_config *addr_config);

void eesb_tx_signal_test(uint8_t channel);
int eesb_rx_data_read(uint8_t *data, uint8_t len);
void eesb_write_payload_pipe(uint8_t pipe, uint8_t *data, uint8_t len, bool flush);
void eesb_set_enable_pipes(uint8_t pipe);

void eesb_tx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config,
					   esb_event_handler event_handler);

void eesb_rx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config,
					   esb_event_handler event_handler);
#endif // __EESB_H__