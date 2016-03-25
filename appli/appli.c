/*
 * appli.c
 *
 *  Created on: 08/2015
 *      Author: S. Poiraud
 */


#include "appli.h"
#include "trellis.h"

/*
 * Initialisation de notrre application, et des périphériques qu'elle utilise
 */
void APPLI_init(void)
{
	//Configuration des broches d'entree-sortie.
	GPIO_Configure();

	//Configuration du port du bouton bleu en entrée
	BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FAST, 0);

	//Initialisation de l'ecran LCD
	STM32f4_Discovery_LCD_Init();

	//Initialisation du touch-screen
	TS_Init();

	//Init i2c
	soft_i2c_init();

	// Init LED driver
	trellis_init();

	// Init encoder
	init_encoder();
}


/*
 * @brief Boucle de tâche de fond de l'application
 * @pre : doit être appelée régulièrement.
 */
void APPLI_process_main(void){

	/*
	 * TIMALLUME LES LEDS
	 */

	trellis_display();

	// On allume qq leds
	// On met à 1 des leds dans le buffer
	/*for (int pg=0;pg<16;pg++) {
		trellis_setLed(pg);
		trellis_display();
	}*/
int i = 0;
int toggle = 0;

while(1){

	trellis_setBrightness(i);
	trellis_setLed(0);
	trellis_setLed(1);
	trellis_setLed(2);
	trellis_setLed(3);
	trellis_setLed(4);
	trellis_setLed(5);
	trellis_setLed(6);
	trellis_setLed(7);
	trellis_setLed(8);
	trellis_setLed(9);
	trellis_setLed(10);
	trellis_setLed(11);
	trellis_setLed(12);
	trellis_setLed(13);
	trellis_setLed(14);
	trellis_setLed(15);

	// On met à jour les LED physiques
	trellis_display();
	if(i<15 && toggle == 0){
		i++;
	}else{
		toggle = 1;
		i--;
		if(i==0){
			toggle = 0;
		}
	}
	for(int j=0;j<500000;j++);
}
	while (1);


	/*
	 * TIME DES DOIGTS
	 */

	/*//Affichage de l'écran d'introduction
	intro_logo();

	//initialisation et lancement de la routine d'interruption sur le timer2
	TIMER2_run_1ms();

	//Loading menu screen
	menu_screen();

	while(1);*/
}

//Cette fonction prend le dessus sur la fonction déclarée dans le fichier timer.c
void TIMER2_user_handler_it_1ms(void)
{
	RTC_process_main(FALSE); // TODO: penser à la pile de sauvegarde lors du routage !!
}
