/**
 * UART.h
 *
 *  Created on: 29 oct. 2013
 *      Author: spoiraud
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx_hal.h"
	#define UART_TIMEOUT 5000

	/* Exported macro ------------------------------------------------------------*/
	#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


	typedef enum
	{
		UART_RECEIVE_ON_MAIN = 0,
		UART_RECEIVE_ON_IT
	}uart_interrupt_mode_e;

	//Initialise l'UART uart_id
	void UART_init(uint8_t uart_id, uart_interrupt_mode_e mode);

	//Deinitialise l'UART uart_id
	void UART_DeInit(uint8_t uart_id);

	//Renvoi le dernier caractere recu ou 0 si pas de caractere disponible
	char UART_getc(uint8_t uart_id);

	//Envoi d'un caractere sur l'UART uart_id
	void UART_putc(uint8_t uart_id, uint8_t c);

#endif /* UART_H_ */
