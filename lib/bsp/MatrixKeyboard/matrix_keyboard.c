/*
 * matrix_keyboard.c
 *
 *  Created on: 24 août 2015
 *      Author: S. Poiraud
 */

#include "matrix_keyboard.h"


#ifdef CONFIG_PULL_UP
	#define DEFAULT_STATE 		(TRUE)
#define CONFIG_PULL	GPIO_PULLUP
#else
	#ifdef CONFIG_PULL_DOWN
		#define DEFAULT_STATE 	(FALSE)
		#define CONFIG_PULL	GPIO_PULLDOWN
	#else
		#error "Vous devez définir CONFIG_PULL_UP ou CONFIG_PULL_DOWN"
	#endif
#endif

//Portage...
#define GPIO_SET_OUTPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FAST, 0})
#define GPIO_SET_INPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_INPUT, CONFIG_PULL, GPIO_SPEED_FAST, 0})
#define GPIO_WRITE					HAL_GPIO_WritePin
#define GPIO_READ					HAL_GPIO_ReadPin


//Fonctions privées
static void KEYBOARD_write_bit_output(uint8_t bit);
static uint8_t  KEYBOARD_get_inputs(void);


//Disposition des touches sur le clavier. (attention, ne correspond pas forcément à la disposition physique dans le bon ordre !)
const char default_keyboard_keys[16] = {
								'D','#','0','*',
								'C','9','8','7',
								'B','6','5','4',
								'A','3','2','1' };

char * keyboard_keys;
static bool_e initialized = FALSE;

/*
@function	Initialise le module keyboard.
@post		Les ports concernés sont configurés en entrés ou en sortie.
*/
void KEYBOARD_init(const char * new_keyboard_keys)
{
	GPIO_SET_OUTPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
	GPIO_SET_OUTPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
	GPIO_SET_OUTPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
	GPIO_SET_OUTPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);

	GPIO_SET_INPUT(PORT_INPUT_0,PIN_INPUT_0);
	GPIO_SET_INPUT(PORT_INPUT_1,PIN_INPUT_1);
	GPIO_SET_INPUT(PORT_INPUT_2,PIN_INPUT_2);
	GPIO_SET_INPUT(PORT_INPUT_3,PIN_INPUT_3);

	KEYBOARD_write_bit_output(DEFAULT_STATE);
	if(new_keyboard_keys)
		keyboard_keys = (char *)new_keyboard_keys;
	else
		keyboard_keys = (char *)default_keyboard_keys;
	initialized = TRUE;
}

/*
@function	Renvoi si une touche au moins est appuyée.
@return		TRUE si une touche est appuyée, FALSE sinon.
*/
bool_e KEYBOARD_is_pressed(void)
{
	uint8_t ret;
	if(!initialized)
		return FALSE;

	KEYBOARD_write_bit_output(!DEFAULT_STATE);

	//Delay

	//Si l'un des ports n'est pas dans l'état par défaut, c'est qu'une touche est pressée.
	ret = (	(GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE));

	KEYBOARD_write_bit_output(DEFAULT_STATE);

	return ret;
}





/*
@function	Renvoi le code ASCII de la touche pressée. En correspondance avec le tableau de codes ASCII.
@post		Cette fonction intègre un anti-rebond
@pre		Il est conseillé d'appeler cette fonction périodiquement (10ms par exemple)
@return		Retourne le caractère ASCII si UNE touche est pressée. Sinon, renvoie 0.
*/
char KEYBOARD_get_key(void)
{
	uint8_t i;
	uint8_t pressed = NO_KEY;
	uint8_t first_pressed_i = NO_KEY;
	uint8_t first_pressed_j = NO_KEY;
	if(!initialized)
		return FALSE;

	for(i=0;i<4;i++)
	{
		KEYBOARD_write_bit_output(DEFAULT_STATE);

		switch(i)
		{
			case 0:	GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,!DEFAULT_STATE);		break;
			case 1:	GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,!DEFAULT_STATE);		break;
			case 2:	GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,!DEFAULT_STATE);		break;
			case 3:	GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,!DEFAULT_STATE);		break;
			default:	return 0;						break;	//Ne doit pas se produire.
		}


		//Acquisition entrées.
		pressed = KEYBOARD_get_inputs();
		if(pressed == MANY_KEYS)
			return MANY_KEYS;
		if(pressed != NO_KEY)	//Si une touche est appuyée.
		{
			if(first_pressed_j != NO_KEY)	//si une touche a déjà été enregistrée
				return NO_KEY;	//On renvoie 0.
			//Sinon on enregistre la touche pressée (ligne et colonne !).
			first_pressed_j = pressed;	//Une touche est appuyée, on la sauvegarde.
			first_pressed_i = i;
		}

	}

	//Fin du balayage.
	if(first_pressed_j == NO_KEY)	//Si aucune touche appuyée.
		return NO_KEY;					//On renvoie 0.

	return keyboard_keys[first_pressed_i*4+first_pressed_j];
}

/*
@function fonction privée. Retourne la lecture des entrées si UNE entrée est à l'état pressée. Sinon, renvoie NO_KEY.
*/
static uint8_t  KEYBOARD_get_inputs(void)
{
	uint8_t inputs;
	inputs = 	((GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE)?1:0)|
				((GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE)?2:0)|
				((GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE)?4:0)|
				((GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE)?8:0);
	switch(inputs)
	{
		case 0b0000:	return NO_KEY;		break;
		case 0b0001:	return 0;			break;
		case 0b0010:	return 1;			break;
		case 0b0100:	return 2;			break;
		case 0b1000:	return 3;			break;
		default:		return MANY_KEYS;	break;	//Plusieurs touches sont appuyées.
	}
}

static void KEYBOARD_write_bit_output(uint8_t bit)
{
	GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,bit);
	GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,bit);
	GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,bit);
	GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,bit);
}
