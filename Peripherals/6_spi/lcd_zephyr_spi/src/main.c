/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "lcd.h"
#include "lcd_init.h"

int main(void)
{
	LCD_Init();
	LCD_Fill(0,0,LCD_W,LCD_H,BLUE);

	LCD_ShowString(20, 50, "Hello World!", RED, WHITE, 32, 0);

	while(1)
	{
		
		for(uint16_t x = 0; x < 240; x++)
		{
			for(uint16_t y = 180; y < 200; y++)
			{
				LCD_DrawPoint(x, y, LIGHTBLUE);
			}
		}

		for(uint16_t x = 0; x < 240; x++)
		{
			for(uint16_t y = 180; y < 200; y++)
			{
				LCD_DrawPoint(x, y, BLUE);
			}
		}
	}
	
	return 0;
}
