#include "lcd_init.h"
#include <zephyr/kernel.h>
#include "hal/nrf_gpio.h"
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>


// Warning: This project requires NCS v2.5.x and above sdk to build.


const struct device *spi= DEVICE_DT_GET(DT_NODELABEL(spi0));

struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE ,
    .frequency = 500000,
    .slave = 0,
    .cs = {
        .gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(reg_dev)),
        .delay = 2,
    }
};

int SPI_init(void)
{
    if(!device_is_ready(spi))
    {
        printk("SPI device is not ready\n");
        return -1;
    }

    return 0;

}

void SPI_write(uint8_t *data, uint16_t len)
{
    struct spi_buf tx_buf = {
        .buf = data,
        .len = len
    };

    struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    int ret = spi_write(spi, &spi_cfg, &tx);
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
    LCD_WR_REG(0x36);
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); // Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15); // GVDD=4.8V  Coler Deep

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); // VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); // 0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
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

    LCD_WR_REG(0xE1);
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
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}
