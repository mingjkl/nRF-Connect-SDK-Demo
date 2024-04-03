#include "lcd_init.h"
#include <zephyr/kernel.h>
#include "nrfx_spim.h"
#include "hal/nrf_gpio.h"
#include <hal/nrf_spim.h>

static nrfx_spim_t spi_instance = NRFX_SPIM_INSTANCE(3);

int SPI_init(void)
{
    nrfx_spim_config_t spi_config = NRFX_SPIM_DEFAULT_CONFIG(LCD_SCLK_PIN, LCD_MOSI_PIN, NRF_SPIM_PIN_NOT_CONNECTED, LCD_CS_PIN);
    spi_config.frequency = NRF_SPIM_FREQ_4M;

    nrfx_spim_init(&spi_instance, &spi_config, NULL, NULL);
}

void SPI_write(uint8_t *data, uint16_t len)
{
    nrfx_spim_xfer_desc_t spi_tx_buff = NRFX_SPIM_XFER_TX(data, len);
    nrfx_spim_xfer(&spi_instance, &spi_tx_buff, 0);
}

void SPI_read(uint8_t *data, uint16_t len)
{
    nrfx_spim_xfer_desc_t spi_rx_buff = NRFX_SPIM_XFER_RX(data, len);
    nrfx_spim_xfer(&spi_instance, &spi_rx_buff, 0);
}


void LCD_GPIO_Init(void)
{
    SPI_init();
    
    nrf_gpio_cfg_output(LCD_RESET_PIN);
    nrf_gpio_cfg_output(LCD_DC_PIN);
    nrf_gpio_cfg_output(LCD_BLK_PIN);

    nrf_gpio_pin_set(LCD_RESET_PIN);
    nrf_gpio_pin_set(LCD_DC_PIN);
    nrf_gpio_pin_set(LCD_BLK_PIN);
}

void LCD_Writ_Bus(uint8_t dat)
{
    SPI_write(&dat, 1);
}

void LCD_WR_DATA8(uint8_t dat)
{
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(uint16_t dat)
{
    uint8_t buffer[2] = {dat >> 8, dat};
    SPI_write(buffer, 2);
}

void LCD_WR_REG(uint8_t dat)
{
    LCD_DC_Clr(); // Write command
    LCD_Writ_Bus(dat);
    LCD_DC_Set(); // Write data
}

void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2a); // Column address set
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // Row address set
        LCD_WR_DATA(y1 + 20);
        LCD_WR_DATA(y2 + 20);
        LCD_WR_REG(0x2c); // RAM write
    }
    else if (USE_HORIZONTAL == 1)
    {
        LCD_WR_REG(0x2a); // Column address set
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b); // Row address set
        LCD_WR_DATA(y1 + 20);
        LCD_WR_DATA(y2 + 20);
        LCD_WR_REG(0x2c); // RAM write
    }
    else if (USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 20);
        LCD_WR_DATA(x2 + 20);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);
    }
    else
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1 + 20);
        LCD_WR_DATA(x2 + 20);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);
    }
}

void LCD_Init(void)
{
    LCD_GPIO_Init(); // Init GPIO
    k_msleep(100);
    LCD_RES_Clr();  // Reset
    k_msleep(100);
    LCD_RES_Set();
    k_msleep(100);

    LCD_BLK_Set(); // Open backlight
    k_msleep(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); // Sleep out
    k_msleep(120);    // Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);   // Memory Data Access Control
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00); 
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);   // Interface Pixel Format
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);   // Porch Setting
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);   // Gate Control
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);   // Vcom Setting
    LCD_WR_DATA8(0x32); // Vcom=1.35V

    LCD_WR_REG(0xC2);   // vdvs
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);   // Vrhs
    LCD_WR_DATA8(0x15); // GVDD=4.8V  Coler Deep

    LCD_WR_REG(0xC4);   // Vdvs
    LCD_WR_DATA8(0x20); // VDV, 0x20:0v

    LCD_WR_REG(0xC6);   // Frctrl2
    LCD_WR_DATA8(0x0F); // 0x0F:60Hz

    LCD_WR_REG(0xD0);   // Power Setting
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);   // Positive Voltage Gamma Control
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);   // Negative Voltage Gamma Control
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0x21);   // Display Inversion On

    LCD_WR_REG(0x29);   // Display On
}
