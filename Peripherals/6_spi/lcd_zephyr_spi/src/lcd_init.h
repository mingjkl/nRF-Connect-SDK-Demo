# ifndef __LCD_INIT_H
# define __LCD_INIT_H

#include <hal/nrf_gpio.h>
#include <zephyr/kernel.h>  

#define USE_HORIZONTAL 0

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280
#else
#define LCD_W 280
#define LCD_H 240
#endif

#define LCD_SCLK_PIN NRF_GPIO_PIN_MAP(0, 3)   // SCL
#define LCD_MOSI_PIN NRF_GPIO_PIN_MAP(0, 4)   // SDA
#define LCD_RESET_PIN NRF_GPIO_PIN_MAP(0, 28) // RES
#define LCD_DC_PIN NRF_GPIO_PIN_MAP(0, 31)    // DC
#define LCD_CS_PIN NRF_GPIO_PIN_MAP(0, 30)    // CS
#define LCD_BLK_PIN NRF_GPIO_PIN_MAP(0, 29)   // BLK

#define LCD_SCLK_Clr() nrf_gpio_pin_clear(LCD_SCLK_PIN)
#define LCD_SCLK_Set() nrf_gpio_pin_set(LCD_SCLK_PIN)

#define LCD_MOSI_Clr() nrf_gpio_pin_clear(LCD_MOSI_PIN)
#define LCD_MOSI_Set() nrf_gpio_pin_set(LCD_MOSI_PIN)

#define LCD_RES_Clr()  nrf_gpio_pin_clear(LCD_RESET_PIN)
#define LCD_RES_Set()  nrf_gpio_pin_set(LCD_RESET_PIN)

#define LCD_DC_Clr()   nrf_gpio_pin_clear(LCD_DC_PIN)
#define LCD_DC_Set()   nrf_gpio_pin_set(LCD_DC_PIN)

#define LCD_CS_Clr()   nrf_gpio_pin_clear(LCD_CS_PIN)
#define LCD_CS_Set()   nrf_gpio_pin_set(LCD_CS_PIN)

#define LCD_BLK_Clr()  nrf_gpio_pin_clear(LCD_BLK_PIN)
#define LCD_BLK_Set()  nrf_gpio_pin_set(LCD_BLK_PIN)

void LCD_GPIO_Init(void);                             // Init GPIO
void LCD_Writ_Bus(uint8_t dat);                            // Simulate SPI write
void LCD_WR_DATA8(uint8_t dat);                            // Write 8 bits
void LCD_WR_DATA(uint16_t dat);                            // Write 16 bits
void LCD_WR_REG(uint8_t dat);                              // Write register
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // Set address
void LCD_Init(void);                                  // Init LCD

# endif