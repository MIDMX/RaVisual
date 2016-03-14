#ifndef __TEXT_H
#define __TEXT_H

#ifdef __cplusplus
 extern "C" {
#endif

const char * footerTxt[10] = {
 	"Menu","Controller",
 	"Session","Checking",
 	"Settings","Password"
 };


//chaînes de caractères correspondant au texte à afficher dans le menu principal
const char * menuText[7] = {
	"CONTROLLER","SESSION",
	"CHECKING","SETTINGS"
};

//chaînes de caractères correspondant au texte à afficher dans le menu settings
const char * list[6] = {
	"Software","Display",
	"Password","Language",
	"Date","Back"
};

//caractères correspondant au texte à affichager sur le clavier
const char * string[60] = {
	"1","2","3","4","5",
	"6","7","8","9","0",
	"A","Z","E","R","T",
	"Y","U","I","O","P",
	"Q","S","D","F","G",
	"H","J","K","L","M",
	"W","X","C","V","B",
	"N","-","-","-"
};


#ifdef __cplusplus
}
#endif

#endif /* __TEXT_H */
