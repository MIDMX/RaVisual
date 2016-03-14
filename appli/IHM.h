#ifndef __IHM_H
#define __IHM_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "macro_types.h"

void header(int bar,bool_e plugged, bool_e armed);

void footer(int choice);

void menu_screen();

void intro_logo();

void setting_menu();

void password_screen();

void draw_setting_selector(int choice);

void draw_menu_selector(int choice);

void draw_keyboard_selector(int choice);

void keyboard_recover_touch(int choice);


struct s_flags{
	char flag;
	void (*p)(int choice);
};
typedef struct s_flags t_flags;


struct x_flags{
	char flag;
	void (*pt)();
};
typedef struct x_flags y_flags;



#ifdef __cplusplus
}
#endif

#endif /* __IHM_H */
