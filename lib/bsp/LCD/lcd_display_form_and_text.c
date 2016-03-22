/**
  ******************************************************************************
  * @file    lcd_display_form_and_text.c
  * @author	 ???
  * @author  modified by Felix Gontier
  * @date    21 juillet 2015
  * @brief   utilisation de l'ecran lcd
  ******************************************************************************
  */

#include "lcd_display_form_and_text.h"
#include "stm32f4_lcd.h"
#include "fonts.h"
#include "macro_types.h"

/** @defgroup stm32f4_discovery_LCD_Private_Variables
  * @{
  */
static sFONT *LCD_Currentfonts = 0;

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
	LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
	return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..n
  * @param back_color: the color of the line background
  * @retval None
  */
void LCD_ClearLine(uint16_t Line, uint16_t back_color)
{
	uint16_t refcolumn = 0;

	do{
		/* Display one character on LCD */
		LCD_DisplayChar(Line, refcolumn, ' ', back_color, back_color);
		/* Decrement the column position by 16 */
		refcolumn += LCD_Currentfonts->Width;
	}while (refcolumn < LCD_PIXEL_WIDTH);
}

/**
  * @brief  Clears the hole LCD.
  * @param  color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t color)
{
	uint32_t index = 0;

	LCD_SetCursor(0x00, 0x00);
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
	for(index = 0; index < LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH; index++)
	{
		LCD_Data = color;
	}
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.
  * @param	color: clor of the pixel tu put
  * @retval None
  */
void LCD_PutPixel(int16_t x, int16_t y, uint16_t color)
{
	if(x < 0 || x > LCD_PIXEL_WIDTH-1 || y < 0 || y > LCD_PIXEL_HEIGHT-1)
	{
		return;
	}
	LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL, color);
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @param	CharColor: color of the character
  * @param	BackColor: color of the background
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c, uint16_t CharColor,  uint16_t BackColor)
{
	uint32_t index = 0, i = 0;
	uint16_t  Xaddress = 0;
	Xaddress = Xpos;
	assert(LCD_Currentfonts);
	LCD_SetCursor(Ypos, Xaddress);

	for(index = 0; index < LCD_Currentfonts->Height; index++)
	{
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		for(i = 0; i < LCD_Currentfonts->Width; i++)
		{
			if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
					(((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
			{
				LCD_WriteRAM(BackColor);
			}
			else
			{
				LCD_WriteRAM(CharColor);
			}
		}
		Xaddress++;
		LCD_SetCursor(Ypos, Xaddress);
	}
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @param	color: color of the character
  * @param	back_color: color of the background
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii, uint16_t color, uint16_t back_color)
{
	Ascii -= 32;
	LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height],color,back_color);
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @param	color: color of the character
  * @param	BackColor: color of the background
  * @param	display_on_uart: can have LCD_DISPLAY_ON_UART or LCD_NO_DISPLAY_ON_UART that define whether or not the string will be sent 
							 through UART.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr, uint16_t color, uint16_t back_color, LCD_display_on_uart_e display_on_uart)
{
	if(display_on_uart == LCD_DISPLAY_ON_UART)
	{
		lcd_to_uart_printf((char*)ptr);
		lcd_to_uart_printf("\n");
	}

	/* Send the string character by character on lCD */
	while (*ptr != 0)
	{
		/* Display one character on LCD */
		LCD_DisplayChar(Line, Column, *ptr, color, back_color);
		/* Decrement the column position by 16 */
		Column += LCD_Currentfonts->Width;
		if (Column >= LCD_PIXEL_WIDTH)
		{
			break;
		}
		/* Point on the next character */
		ptr++;
	}
}



/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position.
  * @param Ypos: specifies the Y position.
  * @param Length: line length.
  * @param Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @param	color: color of the line to be displayed
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction, uint16_t color)
{
	uint32_t i = 0;

	LCD_SetCursor(Xpos, Ypos);
	if(Direction == LCD_DIR_HORIZONTAL)
	{
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		for(i = 0; i < Length; i++)
		{
			LCD_WriteRAM(color);
		}
	}
	else
	{
		for(i = 0; i < Length; i++)
		{
			LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
			LCD_WriteRAM(color);
			Ypos++;
			LCD_SetCursor(Xpos, Ypos);
		}
	}
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @param	color: color of the rectangle to be displayed
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint16_t color)
{
	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_VERTICAL,color);
	LCD_DrawLine((Xpos + Height-1), Ypos, Width, LCD_DIR_VERTICAL,color);

	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_HORIZONTAL,color);
	LCD_DrawLine(Xpos, (Ypos + Width), Height, LCD_DIR_HORIZONTAL,color);
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @param	color: color of the circle to be displayed
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color)
{
	int32_t  D;/* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_SetCursor(Xpos + CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos + CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos - CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos - CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos + CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos + CurY, Ypos - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(color);
		LCD_SetCursor(Xpos - CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    	LCD_WriteRAM(color);
    	LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    	LCD_WriteRAM(color);
    	if (D < 0)
    	{
    		D += (CurX << 2) + 6;
    	}
    	else
    	{
    		D += ((CurX - CurY) << 2) + 10;
    		CurY--;
    	}
    	CurX++;
	}
}



/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
#if 0
  uint32_t index = 0, size = 0;
  /* Read bitmap size */
  size = *(__IO uint16_t *) (BmpAddress + 2);
  size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (BmpAddress + 10);
  index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
  size = (size - index)/2;
  BmpAddress += index;
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(LCD_REG_3, 0x1008);

  LCD_WriteRAM_Prepare();

  for(index = 0; index < size; index++)
  {
    LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
    BmpAddress += 2;
  }

  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  LCD_WriteReg(LCD_REG_3, 0x1018);
#endif
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @param	border_color: color of the border of the rectangle
  * @param	inside_color: color of the inside of the rectangle
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t border_color, uint16_t inside_color)
{
	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL,border_color);
	LCD_DrawLine(Xpos, (Ypos+Height), Width, LCD_DIR_HORIZONTAL,border_color);

	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL,border_color);
	LCD_DrawLine((Xpos+Width-1), Ypos, Height, LCD_DIR_VERTICAL,border_color);

	Height--;
	Ypos++;

	while(Height--)
	{
		LCD_DrawLine(Xpos+1, Ypos++, Width-2, LCD_DIR_HORIZONTAL,inside_color);
	}
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius: specifies the radius of the circle
  * @param	border_color: color of the border of the circle
  * @param	inside_color: color of the inside of the circle
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t border_color, uint16_t inside_color)
{
	int32_t  D;    /* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */

	D = 3 - (Radius << 1);

	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL,inside_color);
			LCD_DrawLine(Xpos + CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL,inside_color);
		}

		if(CurX > 0)
		{
			LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL,inside_color);
			LCD_DrawLine(Xpos + CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL,inside_color);
		}
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
	LCD_DrawCircle(Xpos, Ypos, Radius,border_color);
}

/**
  * @brief  Displays an uni-line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @param	color: specifies the color of the line
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = absolute(x2 - x1);        /* The difference between the x's */
	deltay = absolute(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		LCD_PutPixel(x, y, color);             /* Draw the current pixel */
		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den)             /* Check if numerator >= denominator */
		{
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}
		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}
}
