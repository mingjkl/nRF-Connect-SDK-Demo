/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(RB, LOG_LEVEL_DBG);

#define RING_BUF_SIZE 40

// struct data_ring_buf_t {
// 	struct ring_buf rb;
// 	uint8_t buf[RING_BUF_SIZE];
// };

// static struct data_ring_buf_t my_ring_buf;

RING_BUF_DECLARE(my_ring_buf, RING_BUF_SIZE);

void put_continuous_array(uint8_t cnt, uint8_t base)
{
	uint8_t data[256];
	uint8_t ret;

	LOG_INF("===== PUT %d bytes ===== ", cnt);
	ret = ring_buf_space_get(&my_ring_buf);
	LOG_INF("ring buffer space = %d", ret);

	if(ret < cnt)
	{
		LOG_INF("ring buffer is full");
		LOG_INF("put will be fail");
	}

	for (int i = 0; i < cnt; i++) {
		data[i] = i + base;
	}
	
	LOG_HEXDUMP_INF(data, cnt, "put data");
	ret = ring_buf_put(&my_ring_buf, data, cnt);
	if(ret == cnt)
	{
		LOG_INF("PUT %d bytes success", cnt);
		LOG_HEXDUMP_INF(my_ring_buf.buffer, RING_BUF_SIZE, "ring buffer");
	}
	else
	{
		LOG_INF("PUT %d bytes fail", cnt);
		LOG_INF("PUT only %d bytes", ret);
		LOG_HEXDUMP_INF(my_ring_buf.buffer, RING_BUF_SIZE, "ring buffer");
	}
}

void get_continuous_array(uint8_t cnt)
{
	uint8_t data[256];
	uint8_t ret;

	LOG_INF(" ===== GET %d bytes ===== ", cnt);
	ret = ring_buf_space_get(&my_ring_buf);
	LOG_INF("ring buffer space = %d", ret);

	if(ret < (RING_BUF_SIZE - cnt))
	{
		LOG_INF("ring buffer not enough data");
		LOG_INF("get will be fail");
	}

	ret = ring_buf_get(&my_ring_buf, data, cnt);
	if(ret == cnt)
	{
		LOG_INF("GET %d bytes success", cnt);
		LOG_HEXDUMP_INF(data, cnt, "get data");
		LOG_HEXDUMP_INF(my_ring_buf.buffer, RING_BUF_SIZE, "ring buffer");
	}
	else
	{
		LOG_INF("GET %d bytes fail", cnt);
		LOG_INF("GET only %d bytes", ret);
		LOG_HEXDUMP_INF(my_ring_buf.buffer, RING_BUF_SIZE, "ring buffer");
	}
}

int main(void)
{
	uint8_t ret;

	LOG_INF("Ring buffer example");


	put_continuous_array(10, 0x01);
	k_msleep(1000);
	put_continuous_array(20, 0x21);
	k_msleep(1000);
	get_continuous_array(8);
	k_msleep(1000);
	get_continuous_array(4);
	k_msleep(1000);
	put_continuous_array(16, 0x70);
	k_msleep(1000);
	get_continuous_array(30);
	k_msleep(1000);

	LOG_INF("WARNNIG RING BUFFER AVAILABLE SPACE IS %d", ring_buf_space_get(&my_ring_buf));
	LOG_INF("GET 10 bytes WILL BE FAIL");

	get_continuous_array(10);
	k_msleep(1000);

	put_continuous_array(10, 0xF0);
	k_msleep(1000);

	LOG_INF("WARNNIG RING BUFFER AVAILABLE SPACE IS %d", ring_buf_space_get(&my_ring_buf));
	LOG_INF("PUT 35 bytes WILL BE SUCCESS");

	put_continuous_array(35, 0x80);
	k_msleep(1000);

	
	return 0;
}
