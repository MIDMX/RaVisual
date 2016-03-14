/**
  ******************************************************************************
  * @file    LCD_Touch.h
  * @author  GUERRY Arnaud & GIRAUD Florent
  * @author  Modifi� par Adrien GRAVOUILLE & Simon GESLOT
  * @date    04 F�vrier 2014
  * @brief   Header de LCD_Touch.c
  */

#ifndef _LCD_TOUCH_CALIBRATION_H_
#define _LCD_TOUCH_CALIBRATION_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4_lcd.h"
#include "stmpe811.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "macro_types.h"

/* Exported constants --------------------------------------------------------*/
#define	CROSS_SIZE		10
#define	CROSS_MARGIN	20


/* Exported types ------------------------------------------------------------*/


/**
* \struct TS_ADC
* \brief Structure qui contient les r�sultats de la conversion analogique/num�rique provenant de l'�cran LCD. Ces donn�es sont � l'image de la position de l'endroit press� sur l'�cran.
*/
typedef struct
{
	uint16_t x;
	uint16_t y;
	bool_e touch_detected;
}TS_ADC;


/**
  * @brief  Initialise les ports utilis�s par l'�cran tactile et initialise les coefficients avec des valeurs par d�faut.
  * @pre	cette fonction doit �tre appel�e avant tout appel � l'�cran tactile.
  * @retval None
  */
void TS_Init(void);

bool_e TS_is_initialized(void);

typedef enum
{
	CALIBRATION_MODE_NO_CALIBRATION = 0,			//Initialise seulement les coefficients. (sans lancer une proc�dure de calibration)
	CALIBRATION_MODE_JUST_CALIBRATE,				//Lance une proc�dure de calibration...sans affichage du r�sultat
	CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE,		//Lance une proc�dure de calibration...avec affichage du r�sultat
	CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE_AND_PROVIDE_TELECRAN_GAME	//Lance une proc�dure de calibration...avec affichage du r�sultat et possibilit� de jouer au T�l�cran...
}TS_Calibration_mode_e;
/**
* @brief  Calibre l'�cran tactile
* @pre	Cette fonction doit �tre appel�e en boucle jusqu'� ce qu'elle renvoit TRUE (calibration termin�e).
* @post	Lors de l'�tat INIT, l'appel � TS_Init() est r�alis�.
* @retval END_OK si la calibration est termin�e, IN_PROGRESS sinon.
*/
running_e TS_Calibration(bool_e ask_for_finish, TS_Calibration_mode_e mode);


/**
  * @brief  Retourne les coordonn�es en X et en Y de l'endroit appuy� sur l'�cran
  * @param  ts_adc *  : pointeur vers une structure qui contient les valeurs issues de la conversion analogique num�rique issue du LCD
  * @retval uint16_t : 0 si l'�cran n'est pas appuy�, un entier non nul sinon
  */
bool_e TS_Get_Touch(uint16_t * x, uint16_t * y);


/**
  * @brief  Retourne les valeurs filtr�es (moyenne) issues de la conversion analogique num�rique issue du LCD
  * @param  None
  * @retval TS_ADC * : pointeur vers une structure qui contient les valeurs issues de la conversion analogique num�rique issue du LCD
  */
bool_e TS_Get_Filtered_Touch(TS_ADC * ts_adc);

void TS_process_1ms(void);


#endif
