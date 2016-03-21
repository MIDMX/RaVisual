

#ifndef __LCD_DISPLAY_FORM_AND_TEXT_H
#define __LCD_DISPLAY_FORM_AND_TEXT_H

#include "stm32f4_lcd.h"


void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c, uint16_t CharColor,  uint16_t BackColor);
void LCD_PutPixel(int16_t x, int16_t y, uint16_t color);
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii, uint16_t color, uint16_t back_color);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);

typedef enum
{
	LCD_NO_DISPLAY_ON_UART = 0,
	LCD_DISPLAY_ON_UART
}LCD_display_on_uart_e;

void LCD_DisplayStringLine(uint16_t Line, uint16_t Column, uint8_t *ptr, uint16_t color, uint16_t back_color, LCD_display_on_uart_e display_on_uart);
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction, uint16_t color);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint16_t color);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color);
void LCD_WriteBMP(uint32_t BmpAddress);
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t border_color, uint16_t inside_color);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t border_color, uint16_t inside_color);
void LCD_ClearLine(uint16_t Line, uint16_t back_color);


#endif //__LCD_DISPLAY_FORM_AND_TEXT_H
