/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <nrfx_uarte.h>
#include <hal/nrf_gpio.h>
#include <nrfx_ppi.h>
#include <nrfx_gpiote.h>

#define UARTE_INST_IDX 0

#define RX_BUFFER_SIZE 256
#define RX_END_SYMBOL '\n'

#define UART_TX_PIN NRF_GPIO_PIN_MAP(0, 31)
#define UART_RX_PIN NRF_GPIO_PIN_MAP(0, 8)

#define DEBUG_PIN NRF_GPIO_PIN_MAP(0,30)

static nrfx_uarte_t uarte_instance = NRFX_UARTE_INSTANCE(UARTE_INST_IDX);

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_buffer_index = 0;

uint8_t rx_data;

nrf_ppi_channel_t ppi_channel;
uint8_t gpiote_channel;

static void uart_handler(nrfx_uarte_event_t const * p_event, void * p_context)
{

	switch (p_event->type) {
		case NRFX_UARTE_EVT_TX_DONE:
			NRF_GPIOTE->TASKS_CLR[0] = PPI_TASKS_CHG_EN_EN_Msk | ppi_channel;
			break;
		case NRFX_UARTE_EVT_RX_DONE:
			nrfx_uarte_rx(&uarte_instance, &rx_data, sizeof(rx_data));
			rx_buffer[rx_buffer_index] = rx_data;

			rx_buffer_index++;

			if (rx_data == RX_END_SYMBOL) {
				nrfx_uarte_tx(&uarte_instance, rx_buffer, rx_buffer_index);
				rx_buffer_index = 0;
			}

			if (rx_buffer_index >= RX_BUFFER_SIZE) {
				rx_buffer_index = 0;
			}
			break;
		case NRFX_UARTE_EVT_ERROR:
			break;
		default:
			break;
	}

}


int main(void)
{
	nrfx_err_t err;

	nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG(UART_TX_PIN, UART_RX_PIN);
	uarte_config.p_context = &uarte_instance;
	err = nrfx_uarte_init(&uarte_instance, &uarte_config, uart_handler);
	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_UARTE_INST_GET(UARTE_INST_IDX)), IRQ_PRIO_LOWEST,
                       NRFX_UARTE_INST_HANDLER_GET(UARTE_INST_IDX), 0);
	nrfx_uarte_rx(&uarte_instance, &rx_data, sizeof(rx_data));

	NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos) |
							(DEBUG_PIN << GPIOTE_CONFIG_PSEL_Pos) |
							(GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_POLARITY_Pos);

	nrfx_ppi_channel_alloc(&ppi_channel);

	NRF_PPI->CH[ppi_channel].EEP = (uint32_t)&NRF_UART0->EVENTS_TXDRDY;
	NRF_PPI->CH[ppi_channel].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];

	NRF_PPI->CHENSET = 1 << ppi_channel;

	while (1) {
		k_sleep(K_MSEC(10));
		
	}

	return 0;
}
