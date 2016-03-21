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
#include "ff.h"
#include <math.h>

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
	//if(!LCD_is_initialized())
		//return;
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
	if(!LCD_is_initialized())
		return;
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
	if(1)
	{
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
	//if(!LCD_is_initialized())
		//return;
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
	if(!LCD_is_initialized())
		return;
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

/*  Triangle orienté droite
*  position point supérieur gauche : REFERENCE_X, REFERENCE_Y
*  hauteur du triangle (en pixel) : HAUTEUR_TRIANGLE
*     largeur du triangle (en pixel) : LARGEUR_TRIANGLE
*     couleur                                               :      exple LCD_COLOR_RED
*  *****************************************************
*  *               <--          xpos 1 à 320 -->                        *
*  *  ^                                                                                *
*  *  y                                                                                *
*  *  p 1 à 240                                                                 *
*  *  o                                                                                *
*  *  s                                                                                *
*  *                                                                                  *
*  *****************************************************
*/
bool_e LCD_DrawTriangle(uint16_t xref,uint16_t yref,uint16_t hauteur,uint16_t largeur,uint16_t color)
{
	bool_e ret;
	uint16_t x,y;
	if((0<xref) && (xref<(320-largeur)) && (0<yref) && (yref < (240-hauteur)) )
	{
		for(x=xref;x<(xref+largeur);x++)
		{
			   for(y=yref+(x-xref);y<(yref+hauteur)-(x-xref);y++)
			   {
					  LCD_PutPixel( x, y, color);
			   }
		}
		ret = TRUE;
	}
	else
		ret = FALSE;
	return ret;
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
	if(!LCD_is_initialized())
		return;
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


#if 0
/**
  * @brief  Affiche une image de 320*240 sur le LCD
  * @param  PictureTab : tableau contenant les pixels d'une image.
  * @pre	Ne gère que les images de taille 320*240 !!!!!!!!!!!!!!!!!
  * @retval None
  */
void LCD_WritePicture(uint16_t * BmpAddress)
{

	uint32_t index = 0;
	if(!LCD_is_initialized())
		return;
	/* Read bitmap size */

	/* Set GRAM write direction and BGR = 1 */
	/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	LCD_WriteReg(0x03, 0x1008);
	LCD_WriteRAM_Prepare();

	for(index = 0; index < 320*240; index++)
	{
		LCD_WriteRAM(BmpAddress[index]);
	}

	/* Set GRAM write direction and BGR = 1 */
	/* I/D = 01 (Horizontal : increment, Vertical : decrement) */
	/* AM = 1 (address is updated in vertical writing direction) */
	LCD_WriteReg(0x03, 0x1018);
}
#endif

/**
* \fn void LCD_WriteBMP(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, const uint16_t *bitmap)
* \brief Affiche une image sur l'ecran LCD
*
* \param[in] Xpos uint8_t Position x du coin haut gauche de l'image.
* \param[in] Ypos uint8_t Position y du coin haut gauche de l'image.
* \param[in] Height uint8_t Hauteur de l'image.
* \param[in] Width uint8_t Largeur de l'image.
* \param[in] bitmap uint16_t Pointeur sur l'image.
*
* \return None
*/
void LCD_WritePicture(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, const uint16_t *picture)
{
	uint32_t index;
	uint32_t size = Height * Width;

	if(!LCD_is_initialized())
		return;

	LCD_SetDisplayWindow(Xpos, Ypos, Width-1, Height-1);
	LCD_WriteRAM_Prepare();

	for(index = 0; index < size; index++)
		LCD_WriteRAM(*(__IO uint16_t *)picture++);

	LCD_WriteReg(0x50, 0x0000);
	LCD_WriteReg(0x51, 0x00EF);
	LCD_WriteReg(0x52, 0x0000);
	LCD_WriteReg(0x53, 0x013F);
}


//TODO Test & Debug :

/*
BMP header
image size :320*240
Octets par pixel : 2
*/
const uint8_t bmp_header[70] = {
  0x42, 0x4D, 0x46, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/**
  * @brief  Capture l'image de la mémoire de l'écran LCD et la met sur la carte SD, en BMP
  * @pre	Le support de stockage doit être correctement initialisé...
  * @param  path : le chemin pour le stockage de l'image. Exemple : "0:img.bmp"
  * @retval running_e
  */
running_e Capture_Image_To_BMP(char * path)
{
  FRESULT  ret;
  FIL file;
  uint32_t  i = 0;
  uint32_t  j = 0;
  uint16_t  pixel = 0;
  UINT written = 0;
  uint8_t image_buf[2*LCD_HORIZONTAL_MAX];

	if(!LCD_is_initialized())
		return END_ERROR;
  ret = f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS);
  if (ret != FR_OK)
    return END_ERROR;


  /* write the bmp header */
  ret = f_write(&file, bmp_header, 70, &written);
  if(written != 70)
	  return END_ERROR;

	for (j = LCD_VERTICAL_MAX - 1; j >= 0; j--)
	{
		LCD_SetCursor(0,j);
		LCD_WriteRAM_Prepare();
		for(i=0;i<LCD_HORIZONTAL_MAX;i++)
		{
			pixel = LCD_ReadRAM();
			image_buf[i*2+1] = (uint8_t)(pixel >> 8);
			image_buf[i*2+0] = (uint8_t)(pixel & 0x00ff);
		}
		ret = f_write(&file, image_buf, 2*LCD_HORIZONTAL_MAX, &written);
		if(written != 2*LCD_HORIZONTAL_MAX)
			return END_ERROR;
	}

  ret = f_close(&file);

  if(ret != FR_OK)
	  return END_ERROR;
  return END_OK;
}




/**
  * @brief  Retourne la taille d'un fichier BMP enregistré dans la RAM ou dans la Flash
  * @param  BmpAddress : l'adresse du début du fichier BMP
  * @retval uint32_t : la taille du fichier BMP
  */
uint32_t get_BMP_size(uint8_t * BmpAddress)
{
	return U32FROMU8(BmpAddress[5], BmpAddress[4], BmpAddress[3], BmpAddress[2]);
}


/**
  * @brief  Retourne la largeur d'un fichier BMP enregistré dans la RAM ou dans la Flash
  * @param  BmpAddress : l'adresse du début du fichier BMP
  * @retval uint32_t : la largeur du fichier BMP
  */
uint32_t get_BMP_width(uint8_t * BmpAddress)
{
	return U32FROMU8(BmpAddress[21], BmpAddress[20], BmpAddress[19], BmpAddress[18]);
}


/**
  * @brief  Retourne la hauteur d'un fichier BMP enregistré dans la RAM ou dans la Flash
  * @param  BmpAddress : l'adresse du début du fichier BMP
  * @retval int32_t : la hauteur du fichier BMP
  */
uint32_t get_BMP_height(uint8_t * BmpAddress)
{
	return U32FROMU8(BmpAddress[25], BmpAddress[24], BmpAddress[23], BmpAddress[22]);
}


/**
  * @brief  Retourne le nombre d'octet entre le début du fichier et le début des données d'un fichier BMP enregistré dans la RAM ou dans la Flash
  * @param  BmpAddress : l'adresse du début du fichier BMP
  * @retval int32_t : le nombre d'octet entre le début du fichier et le début des données du fichier BMP
  */
uint32_t get_BMP_data_offset(uint8_t * BmpAddress)
{
	return U32FROMU8(BmpAddress[13], BmpAddress[12], BmpAddress[11], BmpAddress[10]);
}

/**
  * @brief  Retourne le nombre d'octets par pixel BMP enregistré dans la RAM ou dans la Flash
  * @param  BmpAddress : l'adresse du début du fichier BMP
  * @retval int32_t : le nombre d'octets par pixel du fichier BMP
  */
uint16_t get_BMP_bytes_pixel(uint8_t * BmpAddress)
{
	return U16FROMU8(BmpAddress[29], BmpAddress[28])  / 8;
}


/**
  * @brief  Affiche les caractéristiques d'un fichier BMP
  * @param  BmpAddress :  l'adresse du début du fichier BMP
  * @retval None
  */
void write_BMP_details(uint8_t * BmpAddress)
{
	printf("%lu o", get_BMP_size(BmpAddress));
	printf("%lu x %lu", get_BMP_width(BmpAddress),get_BMP_height(BmpAddress));
	printf("%u bytes per pixel", get_BMP_bytes_pixel(BmpAddress));
}



/**
  * @brief  Affiche une image BMP
  * @param  BmpAddress :  l'adresse du début du fichier BMP
  * @param  Xpos :  coordonnée en X du début
  * @param  Ypos :  coordonnée en Y du début
  * @param  Split :  Le facteur de reduction de l'image. Par défaut : 1.
  * @retval None
  */
void LCD_BMP_Resized(uint8_t * BmpAddress, uint16_t Xpos, uint16_t Ypos, uint8_t Split)
{

	uint32_t index = 0, size = 0, width = 0, height = 0, index_fin =0, compt_1, compt_2, width_cut, height_cut;
	uint16_t Valeur_R=0, Valeur_G=0, Valeur_B=0, Valeur_Pixel=0, Bytes_Per_Pixel;
	if(!LCD_is_initialized())
		return;
	/* Read bitmap size */
	size =  get_BMP_size(BmpAddress);
	/* Read bitmap width */
	width = get_BMP_width(BmpAddress);
	/* Read bitmap height */
	height = get_BMP_height(BmpAddress);
	/* Get bitmap data address offset */
	index = get_BMP_data_offset(BmpAddress);
	/*Bytes_Per_Pixel*/
	Bytes_Per_Pixel = get_BMP_bytes_pixel(BmpAddress);

	size = (size - index)/Bytes_Per_Pixel;
	if(Split == 0)
		Split = 1;
	width_cut = width/Split;
	height_cut = height/Split;

	/*Test de non débordement d'écran */
	if((Xpos + width_cut) > 320)
		width_cut=320-Xpos;
	if((Ypos + height_cut) > 240)
		height_cut=240-Ypos;

	//se placer sur la dernière "ligne" de pixels
	index_fin = Bytes_Per_Pixel*width*(height-1);
	index_fin += index;
	// Place L'adresse de l'image BMP à cette dernière ligne.
	BmpAddress += index_fin;

	// Traitement de l'image, conversion si nécessaire

	switch (Bytes_Per_Pixel){
		//Image BMP où les pixels sont codés sur 16 bits en R5G6B5
		case 2 :
			for(compt_1=0; compt_1 < height_cut ; compt_1++ ){
				 LCD_SetCursor(Xpos, Ypos+compt_1);
				 LCD_WriteRAM_Prepare();
					 for(compt_2=0; compt_2 < width_cut ; compt_2++ ){
						 		LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
							BmpAddress += Bytes_Per_Pixel*Split;
					 }
				 BmpAddress -= (Bytes_Per_Pixel*Split)*(width_cut+ width);
			}
		break;
		//Image BMP où les pixels sont codés sur 24 bits en R8G8B8
		case 3 :
			for(compt_1=0; compt_1 < height_cut ; compt_1++ ){
				 LCD_SetCursor(Xpos, Ypos+compt_1);
				 LCD_WriteRAM_Prepare();
					 for(compt_2=0; compt_2 < width_cut ; compt_2++ ){
							Valeur_B = *(__IO uint8_t *)(BmpAddress);
							Valeur_G = *(__IO uint8_t *)(BmpAddress+1);
							Valeur_R = *(__IO uint8_t *)(BmpAddress+2);

							//Application de masques pour ne garder que les bits utiles.
							Valeur_R &= 0x00F8;
							Valeur_G &= 0x00FC;
							Valeur_B &= 0x00F8;

							//Décalage des bits "à leur place"
							Valeur_R <<= 8;
							Valeur_G <<= 3;
							Valeur_B >>= 3;

							//Concaténation des valeurs R5G6B5 obtenues.
							Valeur_Pixel = Valeur_R | Valeur_G | Valeur_B;

							//Ecriture de l'image.
							LCD_WriteRAM(Valeur_Pixel);
							Valeur_Pixel=0;
							BmpAddress += Bytes_Per_Pixel*Split;
					 }
				 BmpAddress -= (Bytes_Per_Pixel*Split)*(width_cut+ width);
			}
			break;
		default :
			;
	}
}


/**
  * @brief  Calcule le rapport de réduction à appliquer pour que l'image BMP tienne dans un rectangle de 106x80
  * @param  BmpAddress :  l'adresse du début du fichier BMP
  * @retval uint8_t : Le rapport de réduction
  */
uint8_t LCD_miniBMP_Split_Calculate(uint8_t * BmpAddress)
{
	if(get_BMP_width(BmpAddress)>106 && get_BMP_height(BmpAddress)>80)
	{
		if(((float)get_BMP_width(BmpAddress)/(float)get_BMP_height(BmpAddress)) <= (float)4/3)
			return ceil((float)get_BMP_height(BmpAddress)/(float)80);
		else
			return ceil((float)get_BMP_width(BmpAddress)/(float)106);
	}
	return 1;
}

/**
  * @brief  Calcule le rapport de réduction à appliquer pour que l'image BMP tienne sur l'écran de 320x240
  * @param  BmpAddress :  l'adresse du début du fichier BMP
  * @retval uint8_t : Le rapport de réduction
  */
uint8_t LCD_BMP_Split_Calculate(uint8_t * BmpAddress)
{
	if(get_BMP_width(BmpAddress)>320 && get_BMP_height(BmpAddress)>240)
	{
		if(((float)get_BMP_width(BmpAddress)/(float)get_BMP_height(BmpAddress)) <= (float)4/3)
			return ceil((get_BMP_height(BmpAddress)/240));
		else
			return ceil((get_BMP_width(BmpAddress)/320));
	}
	return 1;
}



#define HEADERSIZE	40
/**
  * @brief  Affiche une image BMP de taille 320x240 présente sur la carte SD
  * @param  id :  l'identifiant de l'image à afficher
  * @param  x :  la position en X
  * @param  y :  la position en Y
  * @param  size :  la taille de la photo (FULLSIZE : plein écran aux coordonnées (0;0) ou THUMBNAIL : à l'echelle 1/3 pour l'aperçu)
  * @retval None
  */
void Display_BMP_From_SD(char * path, uint8_t id, uint32_t x, uint32_t y, uint8_t size){
	uint8_t buffer[2];
	uint32_t width,height,offset,bytes;
	uint32_t i,j;
	int32_t ret = -1;
	UINT bw = 0;
	FIL file;
	uint8_t header[HEADERSIZE];
	if(!LCD_is_initialized())
		return;
	  ret = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	  if (ret == FR_OK)
	  {
		  ret = f_read(&file, header, HEADERSIZE, &bw);
		  width = get_BMP_width(header);
		  height = get_BMP_height(header);
		  offset = get_BMP_data_offset(header);
		  bytes = get_BMP_bytes_pixel(header);

		  if(width==320 && height==240 && bytes==2)
		  {
			  for(i=HEADERSIZE;i<offset;i++){
				  ret = f_read(&file, buffer, sizeof(uint8_t), &bw);
			  }

			  uint16_t pix =0;

			  if(size==0)	//taille non précisée : plein écran par défaut.
			  {
				  for(j=240;j>0;j--){
					  LCD_SetCursor(0, j-1);
					  LCD_WriteRAM_Prepare();
					  for(i=1;i<=320;i++){
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
						  pix = buffer[1];
						  pix <<= 8;
						  pix |= buffer[0];
						  LCD_WriteRAM(pix);
					  }
				  }
			  }
			  else
			  {
				  for(j=y+80;j>y;j--)
				  {
					  LCD_SetCursor(x, j-1);
					  LCD_WriteRAM_Prepare();
					  for(i=0;i<106;i++)
					  {
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
						  pix = buffer[1];
						  pix <<= 8;
						  pix |= buffer[0];
						  LCD_WriteRAM(pix);
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
					  }

					  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
					  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);

					  for(i=0;i<320;i++)
					  {
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
					  }
					  for(i=0;i<320;i++)
					  {
						  ret = f_read(&file, buffer, sizeof(uint16_t), &bw);
					  }
				  }
			  }
		  }
		  f_close(&file);
	  }
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
	int32_t  CurX;/* Current X Value */
	int32_t  CurY;/* Current Y Value */

	if(!LCD_is_initialized())
		return;
	D = 3 - (Radius << 1);

	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			LCD_DrawLine(Xpos - CurX, Ypos - CurY , 2*CurY, LCD_DIR_VERTICAL,inside_color);
			LCD_DrawLine(Xpos + CurX, Ypos - CurY , 2*CurY, LCD_DIR_VERTICAL,inside_color);
		}

		if(CurX > 0)
		{
			LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL,inside_color);
			LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2*CurX, LCD_DIR_VERTICAL,inside_color);
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

	if(!LCD_is_initialized())
		return;
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
