#ifndef __EESB_H__
#define __EESB_H__

#include "esb.h"

#define EESB_DEFAULT_ADDR                                           \
{                                                                   \
    .base_addr_0 = {0xE7, 0xE7, 0xE7, 0xE7},                        \
    .base_addr_1 = {0xC2, 0xC2, 0xC2, 0xC2},                        \
    .addr_prefix = {0xE7, 0xE7, 0xE7, 0xE7, 0xC2, 0xC2, 0xC2, 0xC2} \
}

struct eesb_addr_config {
    uint8_t base_addr_0[4];
    uint8_t base_addr_1[4];
    uint8_t addr_prefix[8];
};

void eesb_init(struct esb_config *config, struct eesb_addr_config *addr_config);
void eesb_tx_signal_test(uint8_t channel);
void eesb_tx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config);

void eesb_rx_mode_init(enum esb_bitrate bitrate,
					   enum esb_tx_power output_power,
					   struct eesb_addr_config *addr_config);
#endif // __EESB_H__