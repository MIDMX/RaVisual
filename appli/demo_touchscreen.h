/**
  ******************************************************************************
  * @file    LCD_Touch.h
  * @author  GUERRY Arnaud & GIRAUD Florent
  * @author  Modifié par Adrien GRAVOUILLE & Simon GESLOT
  * @date    04 Février 2014
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
* \brief Structure qui contient les résultats de la conversion analogique/numérique provenant de l'écran LCD. Ces données sont à l'image de la position de l'endroit pressé sur l'écran.
*/
typedef struct
{
	uint16_t x;
	uint16_t y;
	bool_e touch_detected;
}TS_ADC;


/**
  * @brief  Initialise les ports utilisés par l'écran tactile et initialise les coefficients avec des valeurs par défaut.
  * @pre	cette fonction doit être appelée avant tout appel à l'écran tactile.
  * @retval None
  */
void TS_Init(void);

bool_e TS_is_initialized(void);

typedef enum
{
	CALIBRATION_MODE_NO_CALIBRATION = 0,			//Initialise seulement les coefficients. (sans lancer une procédure de calibration)
	CALIBRATION_MODE_JUST_CALIBRATE,				//Lance une procédure de calibration...sans affichage du résultat
	CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE,		//Lance une procédure de calibration...avec affichage du résultat
	CALIBRATION_MODE_CALIBRATE_AND_SHOW_VALUE_AND_PROVIDE_TELECRAN_GAME	//Lance une procédure de calibration...avec affichage du résultat et possibilité de jouer au Télécran...
}TS_Calibration_mode_e;
/**
* @brief  Calibre l'écran tactile
* @pre	Cette fonction doit être appelée en boucle jusqu'à ce qu'elle renvoit TRUE (calibration terminée).
* @post	Lors de l'état INIT, l'appel à TS_Init() est réalisé.
* @retval END_OK si la calibration est terminée, IN_PROGRESS sinon.
*/
running_e TS_Calibration(bool_e ask_for_finish, TS_Calibration_mode_e mode);


/**
  * @brief  Retourne les coordonnées en X et en Y de l'endroit appuyé sur l'écran
  * @param  ts_adc *  : pointeur vers une structure qui contient les valeurs issues de la conversion analogique numérique issue du LCD
  * @retval uint16_t : 0 si l'écran n'est pas appuyé, un entier non nul sinon
  */
bool_e TS_Get_Touch(uint16_t * x, uint16_t * y);


/**
  * @brief  Retourne les valeurs filtrées (moyenne) issues de la conversion analogique numérique issue du LCD
  * @param  None
  * @retval TS_ADC * : pointeur vers une structure qui contient les valeurs issues de la conversion analogique numérique issue du LCD
  */
bool_e TS_Get_Filtered_Touch(TS_ADC * ts_adc);

void TS_process_1ms(void);


#endif
