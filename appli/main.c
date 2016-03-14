 /**
  *
  * \file main.c
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "macro_types.h"
#include "main.h"
#include "stm32f4_uart.h"
#include "stm32f4_sys.h"
#include "appli.h"


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @func int main(void)
  * @param  None
  * @retval None
  */
int main(void)
{
	//initialisation du systeme (horloge...)
	HAL_Init();
	SYS_init();
	APPLI_init();

	//appel au process principal
	APPLI_process_main();
}





