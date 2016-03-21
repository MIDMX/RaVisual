/*
 * MatrixKeyboard.h
 *
 *  Created on: 24 août 2015
 *      Author: Nirgal
 */

#ifndef LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#define LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_
#include "stm32f4xx_hal.h"
#include "macro_types.h"


	#define PORT_INPUT_0 	GPIOB
	#define PORT_INPUT_1 	GPIOB
	#define PORT_INPUT_2 	GPIOB
	#define PORT_INPUT_3 	GPIOB
	#define	PIN_INPUT_0		GPIO_PIN_12
	#define	PIN_INPUT_1		GPIO_PIN_13
	#define	PIN_INPUT_2		GPIO_PIN_14
	#define	PIN_INPUT_3		GPIO_PIN_15

	#define PORT_OUTPUT_0 	GPIOE
	#define PORT_OUTPUT_1 	GPIOE
	#define PORT_OUTPUT_2 	GPIOE
	#define PORT_OUTPUT_3 	GPIOE
	#define	PIN_OUTPUT_0	GPIO_PIN_2
	#define	PIN_OUTPUT_1	GPIO_PIN_4
	#define	PIN_OUTPUT_2	GPIO_PIN_5
	#define	PIN_OUTPUT_3	GPIO_PIN_6

	#define MATRIX_KEYBOARD_HAL_CLOCK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE()

	#define CONFIG_PULL_UP
	//#define CONFIG_PULL_DOWN


	/*
	@function	Initialise le module keyboard.
	@post		Les ports concernés sont configurés en entrés ou en sortie.
	*/
	void KEYBOARD_init(const char * new_keyboard_keys);

	/*
	@function	Renvoi si une touche au moins est appuyée.
	@return		TRUE si une touche est appuyée, FALSE sinon.
	*/
	bool_e KEYBOARD_is_pressed(void);



	#define NO_KEY 0xFF
	#define MANY_KEYS 0xFE
	/*
	@function	Renvoi le code ASCII de la touche pressée. En correspondance avec le tableau de codes ASCII.
	@post		Cette fonction intègre un anti-rebond
	@pre		Il est conseillé d'appeler cette fonction périodiquement (10ms par exemple)
	*/
	char KEYBOARD_get_key(void);



#endif /* LIB_BSP_MATRIXKEYBOARD_MATRIX_KEYBOARD_H_ */
