#ifndef __LCD_H
#define __LCD_H

#include "lcd.h"
#include "lcd_init.h"

void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);  // fill a color
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);                                 // draw a point
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);      // draw a line
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color); // draw a rectangle
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                      // draw a circle

void LCD_ShowChinese(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);      // display a Chinese character
void LCD_ShowChinese12x12(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // display a 12x12 Chinese character
void LCD_ShowChinese16x16(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // display a 16x16 Chinese character
void LCD_ShowChinese24x24(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // display a 24x24 Chinese character
void LCD_ShowChinese32x32(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // display a 32x32 Chinese character

void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);        // display a character
void LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode); // display a string
uint32_t mypow(uint8_t m, uint8_t n);                                                                                 // m^n
void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);      // display an integer
void LCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);      // display a float number

void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]); // display a picture

// color
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40      // brown
#define BRRED 0XFC07      // light brown
#define GRAY 0X8430       // gray
#define DARKBLUE 0X01CF   // dark blue
#define LIGHTBLUE 0X7D7C  // light blue
#define GRAYBLUE 0X5458   // gray blue
#define LIGHTGREEN 0X841F // light green
#define LGRAY 0XC618      // light gray (used to display the button)
#define LGRAYBLUE 0XA651  // light gray blue (used to display the button)
#define LBBLUE 0X2B12     // light brown blue (used to display the button)

#endif
