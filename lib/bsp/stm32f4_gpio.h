// TODO : INCLUDES


/***
   * \file GPIO.h
   * \brief fichier .h pour GPIO
   * \author Florent CHARRUAUD
   * \date 23 janvier 2014
   */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifndef GPIO_H_
#define GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* Private function prototypes -----------------------------------------------*/


void GPIO_Configure(void);
void BSP_GPIO_PinCfg(GPIO_TypeDef * GPIOx, uint32_t GPIO_Pin, uint32_t GPIO_Mode, uint32_t GPIO_Pull, uint32_t GPIO_Speed, uint32_t GPIO_Alternate);



#endif /* GPIO_H_ */
