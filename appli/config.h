/*
 * config.h
 *
 *  Created on: 27 oct. 2015
 *      Author: S. Poiraud
 */

#ifndef APPLI_CONFIG_H_
#define APPLI_CONFIG_H_

#define DEMO_WITH_LCD		1		//Indiquer 0 si vous êtes en I1 (activité GPE), 1 sinon.
		/*Cette configuration permet de s'adapter à ces deux situations :
			1 : La carte BaseBoard et un LCD tactile sont branchés
					-> le menu est affiché sur l'UART et sur l'écran, et les fonctionnalités tactiles sont disponibles
			0 : La carte STM32F4-DISCOVERY est nue, ou branchée à un support fait maison
					-> le menu est affiché sur l'UART
		*/

#endif /* APPLI_CONFIG_H_ */
