/**
 * TIMER.h
 *
 *  Created on: 29 oct. 2013
 *      Author: Nirgal
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "stm32f4xx_hal.h"

#include "main.h"	//Permet de désactiver ce module logiciel en définissant DISABLE_BSP_TIMER. Utile à des fins pédagogiques uniquement.
#ifndef DISABLE_BSP_TIMER
/**
 * @brief	Initialisation et lancement du timer 2.
 * 			Cette fonction lance de timer 2 et le configure pour qu'il déclenche sa routine d'interruption toutes les ms.
 * @func 	void TIMER2_run_1ms(void)
 * @post	Le timer 2 et son horloge sont activés, ses interruptions autorisées, et son décompte lancé.
 */
void TIMER2_run_1ms(void);


/**
 * @brief	Fonction de l'utilisateur appelée par la routine d'interruption du timer 2.
 * 			Cette fonction doit etre définie par l'utilisateur... Elle est appelée par la routine d'IT du timer 2.
 * @func 	void TIMER2_user_handler_it_1ms(void)
 * @post	Le timer 2 et son horloge sont activés, ses interruptions autorisées, et son décompte lancé.
 */
void TIMER2_user_handler_it_1ms(void);

#endif
#endif /* TIMER_H_ */
