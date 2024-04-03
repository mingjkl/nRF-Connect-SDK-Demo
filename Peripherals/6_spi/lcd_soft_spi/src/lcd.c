#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"

/**
 * @brief  Fill a color in a specified area
 * @param  xsta,ysta: start coordinates
 * @param  xend,yend: end coordinates
 * @param  color: color to fill
 * @retval None
 */
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // Set the cursor position
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

/**
 * @brief  Draw a point at a specified position
 * @param  x,y: point coordinates
 * @param  color: point color
 * @retval None
 */

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y); // Set the cursor position
    LCD_WR_DATA(color);
}

/**
 * @brief  Draw a line
 * @param  x1,y1: start coordinates
 * @param  x2,y2: end coordinates
 * @param  color: line color
 * @retval None
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // Calculate the difference between the two coordinates
    delta_y = y2 - y1;
    uRow = x1; // Record the initial coordinates
    uCol = y1;
    if (delta_x > 0)
        incx = 1; // Determine the direction of the line
    else if (delta_x == 0)
        incx = 0; // Vertical line
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // Horizontal line
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; // Determine the length of the line
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color); // Draw a point
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief  Draw a rectangle
 * @param  x1,y1: start coordinates
 * @param  x2,y2: end coordinates
 * @param  color: rectangle color
 * @retval None
 */

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

/**
 * @brief  Draw a circle
 * @param  x0,y0: circle center coordinates
 * @param  r: circle radius
 * @param  color: circle color
 * @retval None
 */

void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color); // 3
        LCD_DrawPoint(x0 + b, y0 - a, color); // 0
        LCD_DrawPoint(x0 - a, y0 + b, color); // 1
        LCD_DrawPoint(x0 - a, y0 - b, color); // 2
        LCD_DrawPoint(x0 + b, y0 + a, color); // 4
        LCD_DrawPoint(x0 + a, y0 - b, color); // 5
        LCD_DrawPoint(x0 + a, y0 + b, color); // 6
        LCD_DrawPoint(x0 - b, y0 + a, color); // 7
        a++;
        if ((a * a + b * b) > (r * r)) // Determine the next point
        {
            b--;
        }
        {
            b--;
        }
    }
}



/**
 * @brief  Display a single character
 * @param  x,y: display coordinates
 * @param  num: character to display
 * @param  fc: font color
 * @param  bc: background color
 * @param  sizey: font size
 * @param  mode: 0 non-overlapping mode 1 overlapping mode
 * @retval None
 */
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t temp, sizex, t, m = 0;
    uint16_t i, TypefaceNum; // Number of bytes occupied by a character
    uint16_t x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';                                     // Get the offset value
    LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
    for (i = 0; i < TypefaceNum; i++)
    {
        if (sizey == 12)
            temp = ascii_1206[num][i]; // Call 6x12 font
        else if (sizey == 16)
            temp = ascii_1608[num][i]; // Call 8x16 font
        else if (sizey == 24)
            temp = ascii_2412[num][i]; // Call 12x24 font
        else if (sizey == 32)
            temp = ascii_3216[num][i]; // Call 16x32 font
        else
            return;
        for (t = 0; t < 8; t++)
        {
            if (!mode) // Non-overlapping mode
            {
                if (temp & (0x01 << t))
                    LCD_WR_DATA(fc);
                else
                    LCD_WR_DATA(bc);
                m++;
                if (m % sizex == 0)
                {
                    m = 0;
                    break;
                }
            }
            else // Overlapping mode
            {
                if (temp & (0x01 << t))
                    LCD_DrawPoint(x, y, fc); // Draw a point
                x++;
                if ((x - x0) == sizex)
                {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
    }
}

/**
 * @brief  Display a string
 * @param  x,y: display coordinates
 * @param  *p: string to display
 * @param  fc: font color
 * @param  bc: background color
 * @param  sizey: font size
 * @param  mode: 0 non-overlapping mode 1 overlapping mode
 * @retval None
 */

void LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    while (*p != '\0')
    {
        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

/**
 * @brief  Display a number
 * @param  x,y: display coordinates
 * @param  num: number to display
 * @param  len: number of digits to display
 * @param  fc: font color
 * @param  bc: background color
 * @param  sizey: font size
 * @retval None
 */

void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t sizex = sizey / 2;
    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}


/**
 * @brief  Display a floating point number
 * @param  x,y: display coordinates
 * @param  num: floating point number to display
 * @param  len: number of digits to display
 * @param  fc: font color
 * @param  bc: background color
 * @param  sizey: font size
 * @retval None
*/
void LCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
    uint8_t t, temp, sizex;
    uint16_t num1;
    sizex = sizey / 2;
    num1 = num * 100;
    for (t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10;
        if (t == (len - 2))
        {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/**
 * @brief  Display a picture
 * @param  x,y: display coordinates
 * @param  length,width: picture size
 * @param  pic: picture data
 * @retval None
 */

void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
    uint16_t i, j;
    uint32_t k = 0;
    LCD_Address_Set(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < width; j++)
        {
            LCD_WR_DATA8(pic[k * 2]);
            LCD_WR_DATA8(pic[k * 2 + 1]);
            k++;
        }
    }
}
