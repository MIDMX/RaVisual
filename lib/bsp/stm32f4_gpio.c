// TODO : Alternate Mode


/** \file GPIO.c
   * \brief implementation des fonctions d'initialisation des GPIO et les fonctions d'usage.
   * \author Florent CHARRUAUD
   * \date 23 janvier 2014
   */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_gpio.h"
#include "stm32f4xx_hal.h"





/* Private functions ---------------------------------------------------------*/
/**
 * @brief	Configuration des entrees/sorties des broches du microcontroleur.
 * @func	void GPIO_COnfigure(void)
 * @post	Activation des horloges des peripheriques GPIO, configuration en entree ou en sortie des broches choisies.
 */
void GPIO_Configure(void)
{
	//Activation des horloges des peropheriques GPIOx
	__HAL_RCC_GPIOA_CLK_ENABLE(); // Now defined in macros : stm32f4_hal_rcc.h (417)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//BOUTON
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FAST, 0);
	
	BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0);
}


/**
 * @brief Fonction generale permettant de configurer une broche
 * @func void BSP_GPIO_PinCfg(GPIO_TypeDef * GPIOx, uint32_t GPIO_Pin, uint32_t GPIO_Mode, uint32_t GPIO_Pull, uint32_t GPIO_Speed, uint32_t GPIO_Alternate)
 * @param GPIOx : peut-etre GPIOA-G
 * @param GPIO_Pin : GPIO_PIN_X avec X correspondant a la (ou les) broche souhaitee
 * @param GPIO_Mode : GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD, GPIO_MODE_AF_PP, GPIO_MODE_AF_OD ou GPIO_MODE_ANALOG
 * @param GPIO_Pull : GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN
 * @param GPIO_Speed : GPIO_SPEED_LOW (2MHz), GPIO_SPEED_MEDIUM (25MHz), GPIO_SPEED_FAST (50MHz), GPIO_SPEED_HIGH (100MHz)
 * @param GPIO_Alternate : voir stm32f4xx_hal_gpio_ex.h (Uniquement pour GPIO_Mode = GPIO_MODE_AF_PP ou GPIO_MODE_AF_OD, mettre à 0 sinon
 */
void BSP_GPIO_PinCfg(GPIO_TypeDef * GPIOx, uint32_t GPIO_Pin, uint32_t GPIO_Mode, uint32_t GPIO_Pull, uint32_t GPIO_Speed, uint32_t GPIO_Alternate)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//Structure contenant les arguments de la fonction GPIO_Init

	GPIO_InitStructure.Pin = GPIO_Pin;
	GPIO_InitStructure.Mode = GPIO_Mode;
	GPIO_InitStructure.Pull = GPIO_Pull;
	GPIO_InitStructure.Speed = GPIO_Speed;
	GPIO_InitStructure.Alternate = GPIO_Alternate;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}



