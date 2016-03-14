#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "macro_types.h"
#include "IHM.h"

typedef struct{
	uint8_t x;
	uint8_t y;
	uint32_t width;
	uint8_t height;
}Button_Type;

typedef enum{
	MAIN = 0,
	SESSION = 1,
	CONTROLLER = 2,
	CHECKING = 3,
	SETTINGS = 4,
	PASSWORD = 5
}Menu_Type;


void create_button(Button_Type button[50],int number,int x,int y,int w,int h);

void read_screen(Button_Type button[50],Menu_Type menu, int taille);

#ifdef __cplusplus
}
#endif

#endif /* __CONTROLLER_H */
