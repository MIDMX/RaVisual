/**
 * Uart.c
 *
 *
 *  Created on: 9 sept. 2013
 *      Author: tbouvier
 */

#include "stm32f4xx_hal.h"
#include "stm32f4_uart.h"
#include "stm32f4_gpio.h"
#include "macro_types.h"

#define USART_NB	6
static UART_HandleTypeDef UART_HandleStructure[USART_NB];
const USART_TypeDef * instance_array[USART_NB] = {USART1, USART2, USART3, UART4, UART5, USART6};

/**
 * @brief	Initialise l'USARTx - 115200 bauds - 8N1
 * @func	void UART_init(UART_HandleTypeDef * UART_HandleStructure)
 * @pre 	L'intance de l'UART doit deja etre associee au parametre (UART_HandleStructure)
 * @param	UART_HandleStructure peut recevoir comme instance : USART1, USART2 ou USART6.
 * @param	mode : UART_RECEIVE_ON_MAIN ou UART_RECEIVE_ON_IT (pour activer les interruptions sur réception de caractères)
 * @post	Cette fonction initialise les broches suivante selon l'USART choisit en parametre :
 * 				USART1 : Rx=PC7 et Tx=PB6,   init des horloge du GPIOB et de l'USART1.
 * 				USART2 : Rx=PC3 et Tx=PA2, init des horloge du GPIOA et de l'USART2.
 * 				USART6 : Rx=PC7 et Tx=PC6, init des horloge du GPIOC et de l'USART6.
 * 			parametres d'initialisation de l'USART :
 * 				vitesse de 115200 bauds/sec, 8N1, pas de gestion des envois et reception sur interruption.
 *
 */
void UART_init(uint8_t uart_id, uart_interrupt_mode_e mode)
{
	uint8_t index = uart_id - 1;

	/* USARTx configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
		- OverSampling: enable
	*/
	UART_HandleStructure[index].Instance = (USART_TypeDef*)instance_array[index];
	UART_HandleStructure[index].Init.BaudRate = 115200;
	UART_HandleStructure[index].Init.WordLength = UART_WORDLENGTH_8B;//
	UART_HandleStructure[index].Init.StopBits = UART_STOPBITS_1;//
	UART_HandleStructure[index].Init.Parity = UART_PARITY_NONE;//
	UART_HandleStructure[index].Init.HwFlowCtl = UART_HWCONTROL_NONE;//
	UART_HandleStructure[index].Init.Mode = UART_MODE_TX_RX;//
	UART_HandleStructure[index].Init.OverSampling = UART_OVERSAMPLING_8;//

	/*On applique les parametres d'initialisation ci-dessus */
	if(HAL_UART_Init(&UART_HandleStructure[index]) != HAL_OK)
	{
		///on notifie l'erreur
	}
	
	/*Activation de l'UART */
	__HAL_UART_ENABLE(&UART_HandleStructure[index]);
/*
	if(mode == UART_RECEIVE_ON_IT)
	{
		// On fixe les priorités des interruptions de usart6 PreemptionPriority = 0, SubPriority = 1 et on autorise les interruptions
		HAL_NVIC_SetPriority(USART6_IRQn , 0, 1);
		HAL_NVIC_EnableIRQ(USART6_IRQn);
		__HAL_UART_ENABLE_IT(&UART_HandleStructure[index],UART_IT_RXNE);
	}
*/
}

void USART6_IRQHandler(void)
{
	UART_IT_Buffer(6);
	/*TODO :
	 * uint8_t index = 6 - 1;
	 * HAL_UART_IRQHandler(&instance_array[index]);
	 */
}


/*TODO:
 *

running_e UART_getc_it_mode(uint8_t uart_id, uint8_t * pData, uint16_t size)
{
	running_e ret = IN_PROGRESS;
	uint8_t index = uart_id - 1;
	HAL_UART_Receive_IT(&UART_HandleStructure[index], pData, size);
	switch(err)
	{
		case HAL_OK:
			ret = END_OK;
			break;
		default:
			ret = END_ERROR;
				break;
	}
}
 */


/**
 * @brief 	fonction appelée par les routines d'interruption des USART : elle va chercher l'octet reçu.
 */
void UART_IT_Buffer(uint8_t uart_id)
{
	uint8_t index = uart_id - 1;
	uint8_t c;
	c = (uint8_t)(instance_array[index]-> DR & (uint16_t)0x00FF);
}

void UART_DeInit(uint8_t uart_id)
{
	uint8_t index = uart_id - 1;
	HAL_UART_DeInit(&UART_HandleStructure[index]);
}

//Callback called by hal_uart
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	/*Selon l'instance de l'UART, on defini les broches qui vont bien (voir la doc)*/
	if(huart->Instance == USART1)
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();		//Horloge des broches a utiliser
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_6, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART1); //Configure Tx as AF
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART1); //Configure Rx as AF

		__HAL_RCC_USART1_CLK_ENABLE();		//Horloge du peripherique UART
	}
	else if(huart->Instance == USART2)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();		//Horloge des broches a utiliser
		BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_2, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2);	//Configure Tx as AF
		BSP_GPIO_PinCfg(GPIOA, GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF7_USART2); //Configure Rx as AF

		__HAL_RCC_USART2_CLK_ENABLE();		//Horloge du peripherique UART
	}
	else if (huart->Instance == USART6)
	{
		__HAL_RCC_GPIOC_CLK_ENABLE();		//Horloge des broches a utiliser
		BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_6, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF8_USART6); //Configure Tx as AF
		BSP_GPIO_PinCfg(GPIOC, GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FAST, GPIO_AF8_USART6); //Configure Rx as AF

		__HAL_RCC_USART6_CLK_ENABLE();		//Horloge du peripherique UART
	}
}





/**
 * @brief	Fonction NON blocante qui retourne le dernier caractere reçu sur l'USARTx. Ou 0 si pas de caractere reçu.
 * @func 	char UART_getc(UART_HandleTypeDef * UART_Handle)
 * @param	UART_Handle : UART_Handle.Instance = USART1, USART2 ou USART6
 * @post	Si le caractere reçu est 0, il n'est pas possible de faire la difference avec le cas où aucun caractere n'est reçu.
 * @ret		Le caractere reçu, sur 8 bits.
 */
char UART_getc(uint8_t uart_id)
{
	uint8_t received;
	uint8_t index = uart_id - 1;
	//Si le flag de reception n'est pas leve, on retourne 0*/
	if(__HAL_UART_GET_FLAG(&UART_HandleStructure[index], UART_FLAG_RXNE) == RESET)
		return 0;
	
	/*Si il est leve mais que l'UART a rencontre un probleme, on renvoie 0*/
	if(HAL_UART_Receive(&UART_HandleStructure[index], &received, 1, UART_TIMEOUT) != HAL_OK)
	{
		return 0;
		///gestion de l'erreur
	}
	/*Sinon on renvoie le mot reçu (contenu dans le buffer) */
	else
		return (char) (received);
}

/**
 * @brief	Envoi un caractere sur l'USARTx. Fonction BLOCANTE si un caractere est deja en cours d'envoi.
 * @func 	void UART_putc(UART_HandleTypeDef * UART_Handle, char c)
 * @param	c : le caractere a envoyer
 * @param	USARTx : USART1, USART2 ou USART6
 */
void UART_putc(uint8_t uart_id, uint8_t c)
{
	uint8_t index = uart_id - 1;
	HAL_UART_Transmit(&UART_HandleStructure[index], &c, 1, UART_TIMEOUT);
}


