/**
  ******************************************************************************
  * @file    LCD_Touch.c
  * @author	 2012 Embest Tech. Co., Ltd.
  * @author  Modifié par les binômes GUERRY Arnaud & GIRAUD Florent / Adrien GRAVOUILLE & Simon GESLOT
  * @author  Repris et mis en forme : Samuel Poiraud
  * @date    24 Février 2015
  * @brief   Gestion de l'écran tactile
  ******************************************************************************
  */

#include "demo_touchscreen.h"
#include "macro_types.h"
#include "lcd_display_form_and_text.h"
#include "stm32f4xx_hal.h"

/**
* \struct TS_COEFF
* \brief Structure qui contient les coefficient de calibration de l'écran LCD
*/
typedef struct{
	float scale_x;
	float scale_y;
	float offset_x;
	float offset_y;
}TS_COEFF;

static TS_COEFF ts_coeff;
static volatile uint8_t t;
static bool_e initialized = FALSE;

/**
  * @brief  Initialise les ports utilisés par l'écran tactile et initialise les coefficients avec des valeurs par défaut.
  * @param  none
  * @pre	cette fonction doit être appelée avant tout appel à l'écran tactile.
  * @retval None
  */
void TS_Init(void)
{
	/* Initiation bas niveau du STMPE811QTR */
	IOE_Config();
	ts_coeff = (TS_COEFF){-0.085,0.067,330,-18};
	initialized = TRUE;
}

/**
  * @brief  Retourne l'état d'initialisation du module TS.
  * @param  none
  * @retval TRUE si le module TS est initialisé
  */
bool_e TS_is_initialized(void)
{
	return initialized;
}

/**
  * @brief  Calibre l'écran tactile
  * @param  mode: le mode de calibration	(@ref TS_Calibration_mode_e)
  * @pre	Cette fonction doit être appelée en boucle jusqu'à ce qu'elle renvoit TRUE (calibration terminée).
  * @post	Lors de l'état INIT, l'appel à TS_Init() est réalisé.
  * @retval running_e END_OK si la calibration est terminée, IN_PROGRESS sinon.
  */
running_e TS_Calibration(bool_e ask_for_finish, TS_Calibration_mode_e mode)
{
	typedef enum{
		INIT=0,
		LEFT_CROSS_DISPLAY,
		WAIT_FIRST_PRESS,
		WAIT_FIRST_RELEASE,
		RIGHT_CROSS_DISPLAY,
		WAIT_SECOND_PRESS,
		WAIT_SECOND_RELEASE,
		COMPUTE,
		WAIT_PRESS_TO_EXIT,
		PLAY_TELECRAN,
		CLOSE
	}state_e;

	static state_e state = INIT;
	static TS_ADC first_cross_point, second_cross_point;
	static bool_e asked_for_finish = FALSE;
	TS_ADC trash;
	char* text[35];
	running_e ret;

	ret = IN_PROGRESS;
	if(ask_for_finish)
		asked_for_finish = TRUE;
	switch(state)
	{
		case INIT:
			asked_for_finish = FALSE;
			TS_Init();
			STM32f4_Discovery_LCD_Init();
			LCD_Clear(LCD_COLOR_WHITE);
			LCD_SetFont(&Font12x12);
			if(mode == CALIBRATION_MODE_NO_CALIBRATION)
				state = CLOSE;
			else
			{
				printf("Calibration of Touchscreen (for display DM-LCD25RT)\n");
				state = LEFT_CROSS_DISPLAY;
			}
			break;


		case LEFT_CROSS_DISPLAY:
			LCD_DrawUniLine(CROSS_MARGIN-CROSS_SIZE, 	CROSS_MARGIN, 			CROSS_MARGIN+CROSS_SIZE,CROSS_MARGIN, 			 LCD_COLOR_BLUE);
			LCD_DrawUniLine(CROSS_MARGIN-CROSS_SIZE, 	CROSS_MARGIN-1, 		CROSS_MARGIN+CROSS_SIZE,CROSS_MARGIN-1, 		 LCD_COLOR_BLUE);
			LCD_DrawUniLine(CROSS_MARGIN-CROSS_SIZE,	CROSS_MARGIN+1,			CROSS_MARGIN+CROSS_SIZE,CROSS_MARGIN+1, 		 LCD_COLOR_BLUE);
			LCD_DrawUniLine(CROSS_MARGIN,				CROSS_MARGIN-CROSS_SIZE,CROSS_MARGIN,			CROSS_MARGIN+CROSS_SIZE, LCD_COLOR_BLUE);
			LCD_DrawUniLine(CROSS_MARGIN-1,			 	CROSS_MARGIN-CROSS_SIZE,CROSS_MARGIN-1, 		CROSS_MARGIN+CROSS_SIZE, LCD_COLOR_BLUE);
			LCD_DrawUniLine(CROSS_MARGIN+1,				CROSS_MARGIN-CROSS_SIZE,CROSS_MARGIN+1,	 		CROSS_MARGIN+CROSS_SIZE, LCD_COLOR_BLUE);
			state = WAIT_FIRST_PRESS;
			break;


		case WAIT_FIRST_PRESS:
			if(!t)
			{
				t = 100;
				TS_Get_Filtered_Touch(&first_cross_point);
				if(first_cross_point.touch_detected)
					state = WAIT_FIRST_RELEASE;
			}
			if(asked_for_finish)
				state = CLOSE;
			break;

		case WAIT_FIRST_RELEASE:
			if(!t)
			{
				t = 100;
				if(!TS_Get_Filtered_Touch(&trash))
					state = RIGHT_CROSS_DISPLAY;
			}
			if(asked_for_finish)
				state = CLOSE;
			break;


		case RIGHT_CROSS_DISPLAY:
			LCD_Clear(LCD_COLOR_WHITE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN-CROSS_SIZE,	LCD_PIXEL_HEIGHT-CROSS_MARGIN, 				LCD_PIXEL_WIDTH-CROSS_MARGIN+CROSS_SIZE,  	LCD_PIXEL_HEIGHT-CROSS_MARGIN,				LCD_COLOR_BLUE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN-CROSS_SIZE,	LCD_PIXEL_HEIGHT-CROSS_MARGIN-1,			LCD_PIXEL_WIDTH-CROSS_MARGIN+CROSS_SIZE, 	LCD_PIXEL_HEIGHT-CROSS_MARGIN-1,			LCD_COLOR_BLUE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN-CROSS_SIZE,	LCD_PIXEL_HEIGHT-CROSS_MARGIN+1,			LCD_PIXEL_WIDTH-CROSS_MARGIN+CROSS_SIZE,  	LCD_PIXEL_HEIGHT-CROSS_MARGIN+1,			LCD_COLOR_BLUE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN,  				LCD_PIXEL_HEIGHT-CROSS_MARGIN-CROSS_SIZE,  	LCD_PIXEL_WIDTH-CROSS_MARGIN,  				LCD_PIXEL_HEIGHT-CROSS_MARGIN+CROSS_SIZE,	LCD_COLOR_BLUE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN-1,  			LCD_PIXEL_HEIGHT-CROSS_MARGIN-CROSS_SIZE,  	LCD_PIXEL_WIDTH-CROSS_MARGIN-1, 			LCD_PIXEL_HEIGHT-CROSS_MARGIN+CROSS_SIZE,	LCD_COLOR_BLUE);
			LCD_DrawUniLine(LCD_PIXEL_WIDTH-CROSS_MARGIN+1,  			LCD_PIXEL_HEIGHT-CROSS_MARGIN-CROSS_SIZE,  	LCD_PIXEL_WIDTH-CROSS_MARGIN+1,  			LCD_PIXEL_HEIGHT-CROSS_MARGIN+CROSS_SIZE,	LCD_COLOR_BLUE);
			state = WAIT_SECOND_PRESS;
			break;

		case WAIT_SECOND_PRESS:
			if(!t)
			{
				t = 100;
				TS_Get_Filtered_Touch(&second_cross_point);
				if(second_cross_point.touch_detected)
					state = COMPUTE;
			}
			if(asked_for_finish)
				state = CLOSE;
			break;

		case COMPUTE:
			ts_coeff.scale_x = (float)((LCD_PIXEL_WIDTH-2*CROSS_MARGIN))	/	((float)second_cross_point.x-(float)first_cross_point.x);
			ts_coeff.scale_y = (float)((LCD_PIXEL_HEIGHT-2*CROSS_MARGIN))	/	((float)second_cross_point.y-(float)first_cross_point.y);

			ts_coeff.offset_x = (float)(CROSS_MARGIN-ts_coeff.scale_x*(float)first_cross_point.x);
			ts_coeff.offset_y = (float)(CROSS_MARGIN-ts_coeff.scale_y*(float)first_cross_point.y);

			if(mode >= CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE)
			{
				LCD_Clear(LCD_COLOR_WHITE);
				sprintf((char*)text,"First cross x = %d  ", first_cross_point.x);
				LCD_DisplayStringLine(LINE(3),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"Second cross x = %d  ", second_cross_point.x);
				LCD_DisplayStringLine(LINE(5),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"First cross y = %d  ", first_cross_point.y);
				LCD_DisplayStringLine(LINE(4),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"Second cross y = %d  ", second_cross_point.y);
				LCD_DisplayStringLine(LINE(6),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);

				LCD_DisplayStringLine(LINE(8),COLUMN(0),(uint8_t *)"Toucher pour continuer !",LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);

				sprintf((char*)text,"Scale x = %f",ts_coeff.scale_x);	//%ld.%03ld  ", (int32_t)ts_coeff.scale_x,(int32_t)(ts_coeff.scale_x*1000)-(1000*(int32_t)ts_coeff.scale_x));
				LCD_DisplayStringLine(LINE(10),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"Offset x = %f",ts_coeff.offset_x);	// = %ld.%03ld  ", (int32_t)ts_coeff.scale_x,(int32_t)(ts_coeff.offset_x*1000)-(1000*(int32_t)ts_coeff.offset_x));
				LCD_DisplayStringLine(LINE(11),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"Scale y = %f",ts_coeff.scale_y);	// = %ld.%03ld  ", (int32_t)ts_coeff.scale_x,(int32_t)(ts_coeff.scale_y*1000)-(1000*(int32_t)ts_coeff.scale_y));
				LCD_DisplayStringLine(LINE(12),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				sprintf((char*)text,"Offset y = %f",ts_coeff.offset_y);	// = %ld.%03ld  ", (int32_t)ts_coeff.scale_x,(int32_t)(ts_coeff.offset_y*1000)-(1000*(int32_t)ts_coeff.offset_y));
				LCD_DisplayStringLine(LINE(13),COLUMN(0),(uint8_t *)text,LCD_COLOR_BLACK, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);

				if(mode == CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE_AND_PROVIDE_TELECRAN_GAME)
				{
					LCD_DisplayStringLine(LINE(15),COLUMN(0),(uint8_t *)"To quit the Telecran Game,",LCD_COLOR_RED, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
					LCD_DisplayStringLine(LINE(16),COLUMN(0),(uint8_t *)"Just press the blue button",LCD_COLOR_RED, LCD_COLOR_WHITE,LCD_DISPLAY_ON_UART);
				}
			}
			state = WAIT_SECOND_RELEASE;
			break;

		case WAIT_SECOND_RELEASE:
			if(!t)
			{
				t = 100;
				if(!TS_Get_Filtered_Touch(&trash))
				{
					switch(mode)
					{
						case CALIBRATION_MODE_JUST_CALIBRATE:
							state = CLOSE;
							break;
						case CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE:
							state = WAIT_PRESS_TO_EXIT;
							break;
						case CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE_AND_PROVIDE_TELECRAN_GAME:
							state = WAIT_PRESS_TO_EXIT;
							break;
						default:
							state = CLOSE;
							break;
					}
				}
			}
			if(asked_for_finish)
				state = CLOSE;
			break;

		case WAIT_PRESS_TO_EXIT:
			if(!t)
			{
				t = 100;
				if(TS_Get_Filtered_Touch(&trash))
				{
					if(mode == CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE)
					{
						LCD_Clear(LCD_COLOR_WHITE);
						state = CLOSE;
					}
					else	//CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE_AND_PROVIDE_TELECRAN_GAME
					{
						LCD_Clear(LCD_COLOR_BLACK);
						state = PLAY_TELECRAN;
					}
				}
			}
			if(asked_for_finish)
				state = CLOSE;
			break;
		case PLAY_TELECRAN:{
			uint16_t x,y;
			char str[30];
			if(!t)
			{
				t = 15;
				if(TS_Get_Touch(&x,&y))
				{
					sprintf((char*)str,"x = %5d",x);
					LCD_DisplayStringLine(LINE(1),COLUMN(0),(uint8_t *)str, LCD_COLOR_WHITE, LCD_COLOR_BLACK,LCD_NO_DISPLAY_ON_UART);
					sprintf((char*)str,"y = %5d",y);
					LCD_DisplayStringLine(LINE(2),COLUMN(0),(uint8_t *)str, LCD_COLOR_WHITE, LCD_COLOR_BLACK,LCD_NO_DISPLAY_ON_UART);
					LCD_PutPixel(x, y, LCD_COLOR_YELLOW);
				}
			}
			if(asked_for_finish)
				state = CLOSE;
			break;}

		case CLOSE:
			LCD_Clear(LCD_COLOR_WHITE);
			state = INIT;
			ret = END_OK;
			break;
		default:
			break;
	}

	return ret;
}


/**
  * @brief  Retourne les coordonnées en X et en Y de l'endroit appuyé sur l'écran
  * @param  x : abscisse à mettre à jour
  * @param  y : ordonnée à mettre à jour
  * @retval _Bool : FALSE si l'écran n'est pas appuyé, TRUE sinon.
  */
bool_e TS_Get_Touch(uint16_t * x, uint16_t * y)
{
	TS_ADC touch;

	if(!initialized)
		return FALSE;

	TS_Get_Filtered_Touch(&touch);

	if(touch.touch_detected)
	{
		*x = MIN((uint16_t)(ts_coeff.scale_x * (float)touch.x + ts_coeff.offset_x) ,LCD_PIXEL_WIDTH);
		*y = MIN((uint16_t)(ts_coeff.scale_y * (float)touch.y + ts_coeff.offset_y) ,LCD_PIXEL_HEIGHT);
	}

	return touch.touch_detected;
}

/**
  * @brief  Donne au parametre les valeurs filtrées (moyenne) issues de la conversion analogique numérique issue du LCD et retourne si un appuie a ete detecte
  * @param  *ts_adc
  * @retval bool_e: pointeur vers une structure qui contient les valeurs issues de la conversion analogique numérique issue du LCD
  */
bool_e TS_Get_Filtered_Touch(TS_ADC * ts_adc)
{
	TS_STATE *pstate = NULL;

	pstate = IOE_TS_GetState();
	ts_adc->touch_detected = pstate->TouchDetected;
	if(ts_adc->touch_detected)
	{
		ts_adc->x = pstate->X;
		ts_adc->y = pstate->Y;
	}
	else
	{
		ts_adc->x = 0;
		ts_adc->y = 0;
	}

	return ts_adc->touch_detected;
}

/**
  * @brief  Fonction appelee toutes les ms par la routine d'interruption du timer2
  * @param  none
  * @retval none
  */
void TS_process_1ms(void)
{
	if(t)
		t--;
}
