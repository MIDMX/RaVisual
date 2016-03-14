/*
 * appli.c
 *
 *  Created on: 08/2015
 *      Author: S. Poiraud
 */


#include "appli.h"

/*
 * Initialisation de notre application, et des p�riph�riques qu'elle utilise
 */
void APPLI_init(void)
{
	//Configuration des broches d'entree-sortie.
	GPIO_Configure();

	//Configuration du port du bouton bleu en entr�e
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FAST, 0);

	//Initialisation de l'ecran LCD
	STM32f4_Discovery_LCD_Init();

	//Initialisation du touch-screen
	TS_Init();
}


/*
 * @brief Boucle de t�che de fond de l'application
 * @pre : doit �tre appel�e r�guli�rement.
 */
void APPLI_process_main(void){

	//Affichage de l'�cran d'introduction
	intro_logo();

	//initialisation et lancement de la routine d'interruption sur le timer2
	TIMER2_run_1ms();

	//Loading menu screen
	menu_screen();

	while(1);
}

//Cette fonction prend le dessus sur la fonction d�clar�e dans le fichier timer.c
void TIMER2_user_handler_it_1ms(void)
{
	RTC_process_main(FALSE); // TODO: penser � la pile de sauvegarde lors du routage !!
}
