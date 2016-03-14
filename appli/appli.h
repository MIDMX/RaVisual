/*
 * appli.h
 *
 *  Created on: 1 avr. 2015
 *      Author: Nirgal
 */

#ifndef APPLI_H_
#define APPLI_H_

#include "stm32f4xx_hal.h"
#include "stm32f4_gpio.h"
#include "stm32f4_uart.h"
#include "stm32f4_timer.h"
#include "stm32f4_sys.h"
#include "stm32f4_rtc.h"
#include "stm32f4_lcd.h"
#include "demo_touchscreen.h"
#include "macro_types.h"

#include "lcd_display_form_and_text.h"
#include "fonts.h"

#include <string.h>
#include <stdio.h>

#include "IHM.h"
#include "stm32f4xx_hal.h"

/*
 * @brief Initialisation de notre application, et des périphériques qu'elle utilise.
 * @pre : cette fonction doit être appelée préalablement à tout autre appel de fonction de cette classe.
 */
void APPLI_init(void);

/*
 * @brief Boucle de tâche de fond de l'application
 * @pre : doit être appelée régulièrement.
 */
void APPLI_process_main(void);

/*
 * @brief Routine d'it de l'application.
 * @pre : cette fonction doit être appelée toutes les ms.
 */
void APPLI_process_1ms(void);

/* Defines */
#define BLUE_LED GPIOD, GPIO_PIN_15
#define GREEN_LED GPIOD, GPIO_PIN_12
#define ORANGE_LED GPIOD, GPIO_PIN_13
#define BLUE_BUTTON	GPIOA, GPIO_PIN_0

#endif /* APPLI_H_ */
