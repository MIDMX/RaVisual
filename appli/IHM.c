#include "IHM.h"
#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "stm32f4_lcd.h"
#include "lcd_display_form_and_text.h"
#include "images.h"
#include "text.h"
#include "controller.h"
#include <stdio.h>
#include <string.h>

//Définition de la chaine contenant l'heure en "extern"
//On pourra donc l'utiliser n'importe quand et n'importe ou
extern char rtctime[20];
Button_Type button[50];
char * password[20];

void header(int bar,bool_e plugged, bool_e armed){

	//définition des variables
	uint16_t i;
	uint16_t x = 241+40;

	//Contours+fond d'écran
	create_rectangle(0,0,320,25,0x2104);

	//affichage de l'icone usb si système branché
	if(plugged == TRUE){
		DISP_BMP((uint8_t*)usb,27,0,32,24);
	}

	//affichage de l'icone armed si système branché
	if(armed == TRUE){
		DISP_BMP((uint8_t*)key,0,1,32,24); //TODO : FIX Y AXIS PROBLEM
	}

	//logo batterie
	create_rectangle(239+40,6,28,13,0xFFFF);
	create_rectangle(240+40,7,26,11,0x2104);
	create_rectangle(266+40,9,3,7,0xFFFF);

	//boucle remplissage
	for (i=0;i<bar;i++){
		create_rectangle(x,8,4,9,0xFFFF);
		x = x+5;
	}

	//Choix de la taille de police pour l'écriture dans le header
	LCD_SetFont(&Bebas);
}

void footer(int choice){

	//Création de la barre de footer
	create_rectangle(0,215,320,25,0x2104);

	//affichage du triangle
	DISP_BMP((uint8_t*)triangle,5,215,13,7);

	//Choix de la taille de police pour l'écriture dans le header
	LCD_SetFont(&Bebasn);

	//display text information about the screen
	LCD_DisplayStringLine(226,8,(uint8_t *)footerTxt[choice],0xFFFF,0x2104,LCD_NO_DISPLAY_ON_UART);

	//Ré-affichage de l'heure pour assurer son apparition dans le cas ou un raffraichissement est effectué avant la prochaine seconde
	LCD_DisplayStringLine(226,265,rtctime,0xffff,0x2104,LCD_NO_DISPLAY_ON_UART);
}

void intro_logo(){
	//initialisation des variable
	int i;

	//affichage du logo d'introduction
	DISP_BMP((uint8_t*)logo,0,0,320,240);

	//on attend 1 seconde
	Delay(1000);
}

void password_screen(){

	//fond d'écran, header, footer
	create_rectangle(0,0,320,240,0x632C);
	header(5,TRUE,TRUE);

	//initialisatioin des variables
	char * temp[20],hidden[20]={};
	uint32_t i,j,len = strlen(password);
	uint32_t plus = 0, offsetY = 85, offsetX = 10;

	//création de la barre de texte
	LCD_DrawFullRect(10,40,300,32,0x39C7,0xD6BA);
	LCD_DrawFullRect(279,41,30,30,0xD6BA,0x153F);
	create_button(button,41,279,41,30,30);

	//on affiche le mot de passe en masuqnat les caractères
	for(i = 0; i<len;i++){
		strcpy(temp,hidden);
		sprintf(hidden,"%s-",(uint8_t *)temp);
	}
	LCD_DisplayStringLine(52,15,(uint8_t *)hidden,0x39C7,0xD6BA,LCD_NO_DISPLAY_ON_UART);

	//on choisit la police du texte à afficher
	LCD_SetFont(&Bebasn);

	//affichage des icônes dans le menu
	for(i=0;i<4;i++){
		for(j=0;j<10;j++){

			//bufferisation de la position des boutons
			create_button(button,plus,(30*j)+offsetX,(31*i)+offsetY,30,30);

			//drawing separators
			LCD_DrawRect((30*j)+offsetX,(31*i)+offsetY,30,30,0x39C7);
			LCD_DrawFullRect((30*j)+offsetX,(31*i)+offsetY,29,29,0x738E,0x5ACB);
			LCD_DisplayStringLine((31*i)+offsetY+11,(30*j)+offsetX+11,(uint8_t *)string[plus],0xD6BA,0x5ACB,LCD_NO_DISPLAY_ON_UART);
			plus++;

			// on sort de la boucle pour ne pas afficher les deux dernières touches du clavier
			// (choix graphique, on aurait pu faire sans)
			if (plus > 37){
				j = 10;
				i = 4;
			}

		}
	}

	//affichage du footer, puis lecture de l'écran tactile
	footer(PASSWORD);
	read_screen(button,PASSWORD,40);

}

void menu_screen(){

	//initialisation des variables
	int i,j,text = 0;

	//fond d'écran, header, footer
	create_rectangle(0,0,320,240,0x5ACB);
	header(5,FALSE,TRUE);

	//Choix de la taille de police pour l'écriture dans le header
	LCD_SetFont(&Bebasn);

	//affichage des icônes dans le menu
	for(i=0;i<2;i++){
		for(j=0;j<3;j++){
			if(i == 1 && j > 0){
				//DO NOTHING
			}else{
				LCD_DrawFullRect((104*j)+5,(84*i)+38,102,81,0x2104,0xFFFF);
				LCD_DrawFullRect((104*j)+5,(84*i)+38,102,20,0x2104,0x2104);
				LCD_DisplayStringLine((84*i)+44,(105*j)+10,(uint8_t *)menuText[text],0xffff,0x2104,LCD_NO_DISPLAY_ON_UART);
				DISP_BMP((uint8_t*)images[text],(104*j)+6,(84*i)+59,100,60);

				//bufferisation de la position des boutons
				create_button(button,text,(104*j)+5,(84*i)+38,102,81);

				text++;
			}
		}
	}

	footer(MAIN);

	//on lance l'acquisition sur l'écran tactile
	read_screen(button,MAIN,4);
}

void setting_menu(){

	//fond d'écran, header
	header(5,TRUE,TRUE);

	//choix de la police
	LCD_SetFont(&Bebasn);

	//initialisation des variables
	int i = 0, taille = 6;
	const int offsetY = 36;

	//filling text container
	create_rectangle(0,25,320,190,0x5ACB); //TODO: inutile, à virer

	//affichage du menu-list dans le menu settings
	for(i = 0;i<taille;i++){

		//drawing separators
		LCD_DrawLine(0,(32*i)+24,320,LCD_DIR_HORIZONTAL,0x39C7);
		LCD_DrawLine(0,(32*i)+25,320,LCD_DIR_HORIZONTAL,0x738E);

		//displaying text and icons in "non selected color"
		LCD_DisplayStringLine((32*i)+offsetY,45,(uint8_t *)list[i],0xD6BA,0x5ACB,LCD_NO_DISPLAY_ON_UART);
		DISP_BMP((uint8_t*)logoff[i],10,(32*i)+28,24,24);

		//bufferisation de la position des boutons
		create_button(button,i,0,(i*32)+25,320,33);

	}

	//affichage du footer, puis lecture de l'écran tactile
	footer(SETTINGS);
	read_screen(button,SETTINGS,taille);

}

void draw_keyboard_selector(int choice){

	//initialisation de l'écran tactile
	char * temp[20]={},hidden[20]={};
	int i, len = strlen(password);

	//on redessine la touche pressée en style "appuyé"
	LCD_DrawFullRect(button[choice].x,button[choice].y,button[choice].width,button[choice].height,0x39C7,0x39C7);
	LCD_DisplayStringLine(button[choice].y+11,button[choice].x+11,(uint8_t *)string[choice],0xD6BA,0x39C7,LCD_NO_DISPLAY_ON_UART);

	//masquage des précédents caractères composant le mot de passe
	if(len<20){
		for(i=0;i<len;i++){
			strcpy(temp,hidden);
			sprintf(hidden,"%s-",(uint8_t *)temp);
		}
		sprintf(password,"%s%s",(uint8_t *)hidden,(uint8_t *)string[choice]);
	}

	LCD_DisplayStringLine(52,15,(uint8_t *)password,0x39C7,0xD6BA,LCD_NO_DISPLAY_ON_UART);

}

void draw_setting_selector(int choice){

	//initialisation des varibles
	const int offsetY = 36;

	//placing scroller on selection
	create_rectangle(0,(choice*32)+25,320,31,0x39C7);
	create_rectangle(0,(choice*32)+25,2,31,0x153F);

	//changing text and icon color on selection
	LCD_DisplayStringLine((32*choice)+offsetY,45,(uint8_t *)list[choice],0xFFFF,0x39C7,LCD_NO_DISPLAY_ON_UART);
	DISP_BMP((uint8_t*)logon[choice],10,(32*choice)+28,24,24);
}



void draw_menu_selector(int choice){

	//affichage du bouton en mode "illuminé
	LCD_DrawFullRect(button[choice].x,button[choice].y,button[choice].width,button[choice].height,0xFFFF,0xFFFF);
	LCD_DisplayStringLine(button[choice].y+6,button[choice].x+5,(uint8_t *)menuText[choice],0x2104,0xffff,LCD_NO_DISPLAY_ON_UART);
	DISP_BMP((uint8_t*)images[choice],button[choice].x+1,button[choice].y+21,100,60);

}
