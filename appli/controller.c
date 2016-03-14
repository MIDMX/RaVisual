#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "controller.h"
#include "demo_touchscreen.h"
#include "IHM.h"

void create_button(Button_Type button[50],int number,int x,int y,int w,int h){

	button[number].x = x;
	button[number].y = y;
	button[number].width = w;
	button[number].height = h;

}

void read_screen(Button_Type button[50],Menu_Type menu,int taille){

	int i,choice;
	bool_e finish = FALSE, none = FALSE;
	uint16_t touchX, touchY;

	//� d�clarer en statique imp�rativement pour �viter un d�bordement de la pile � chaque appel de la fonction
	static t_flags selector_choice[6] = {
			{'draw_menu_selector',draw_menu_selector},
			{'draw_setting_selector',draw_setting_selector},
			{'draw_setting_selector',draw_setting_selector},
			{'draw_setting_selector',draw_setting_selector},
			{'draw_setting_selector',draw_setting_selector},
			{'draw_keyboard_selector',draw_keyboard_selector}
	};

	//� d�clarer en statique imp�rativement pour �viter un d�bordement de la pile � chaque appel de la fonction
 	static y_flags screen_choice[10][40];

	//main menu function selector table
	screen_choice[MAIN][0].flag = 'password_screen';
	screen_choice[MAIN][0].pt = password_screen;
	screen_choice[MAIN][1].flag = 'setting_menu';
	screen_choice[MAIN][1].pt = setting_menu;
	screen_choice[MAIN][2].flag = 'setting_menu';
	screen_choice[MAIN][2].pt = setting_menu;
	screen_choice[MAIN][3].flag = 'setting_menu';
	screen_choice[MAIN][3].pt = setting_menu;

	//settings function screen table
	screen_choice[SETTINGS][0].flag = 'menu_screen';
	screen_choice[SETTINGS][0].pt = menu_screen;
	screen_choice[SETTINGS][1].flag = 'menu_screen';
	screen_choice[SETTINGS][1].pt = menu_screen;
	screen_choice[SETTINGS][2].flag = 'password_screen';
	screen_choice[SETTINGS][2].pt = password_screen;
	screen_choice[SETTINGS][3].flag = 'menu_screen';
	screen_choice[SETTINGS][3].pt = menu_screen;
	screen_choice[SETTINGS][4].flag = 'menu_screen';
	screen_choice[SETTINGS][4].pt = menu_screen;
	screen_choice[SETTINGS][5].flag = 'menu_screen';
	screen_choice[SETTINGS][5].pt = menu_screen;

	//settings function screen table
	for(i=0;i<39;i++){
		screen_choice[PASSWORD][i].flag = 'password_screen';
		screen_choice[PASSWORD][i].pt = password_screen;
	}


	//boucle de d�tection d'un appui
	//d�tecte quel bouton � �t� press�
	while(!finish){

		if(TS_Get_Touch(&touchX, &touchY)){ 					//si appui d�tect�

			for(i=0;i<taille;i++){ 								//on parcours le tableau contenant la position de chaque �l�ment interactif

				if(touchX >= button[i].x && touchX <= (button[i].width+button[i].x) && touchY >= button[i].y && touchY <= (button[i].height+button[i].y)){ // on v�rifie que l'appui a �t� effectu� sur un �l�ment interactif

					selector_choice[menu].p(i);

					// on attend un rel�chement en faisant deux v�rifications sur un l�ger interval de temps
					// afin de s'assurer que l'�cran � bel et bien �t� rel�ch�
					while(!none){
						if(!TS_Get_Touch(&touchX, &touchY)){
							Delay(50);
							if(!TS_Get_Touch(&touchX, &touchY)) none = TRUE;
						}
					}

					choice = i;								//le choix de l'�l�ment s�lectionn� est attribu� � la variable choice
					finish = TRUE;							//on acquite la boucle de d�tection

				}
			}
		}
	}

	//on entre dans la fonction associ�e au bouton
	screen_choice[menu][choice].pt();

}
