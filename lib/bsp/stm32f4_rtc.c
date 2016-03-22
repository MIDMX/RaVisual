/*
 * stm32f4_rtc.c
 *
 *  Created on: 21 août 2015
 *      Author: Nirgal
 */
#include "stm32f4_rtc.h"
#include "lcd_display_form_and_text.h"
#include <string.h>
#include "stdio.h"
#include "fonts.h"
#include <stdlib.h> /* NULL, malloc() and free() call */


//______________________________________________

/**	running_e DEMO_RTC_process_main(void)	 // Fonction de démonstration (partielle) de l'utilisation de ce module.
 * @brief exemple d'utilisation du module RTC.
 */
running_e DEMO_RTC_process_main(bool_e ask_for_finish)
{
	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	running_e ret;
	ret = IN_PROGRESS;

	switch(state)
	{
		case INIT: {
			state = RUN;
			RTC_init(FALSE);	//Oscillateur INTERNE, imprécis, mais testable sans ajout d'un oscillateur sur PC14 et PC15 !
			//RTC_init(TRUE);		//Oscillateur EXTERNE, précis, mais nécessite l'ajout d'un oscillateur sur PC14 et PC15 !

			//	1/04/2015	12h34'56"
			RTC_TimeTypeDef time = {12, 34, 56, 00, RTC_HOURFORMAT12_AM, RTC_DAYLIGHTSAVING_NONE, RTC_STOREOPERATION_RESET};
			RTC_DateTypeDef date = {RTC_WEEKDAY_MONDAY, RTC_MONTH_APRIL, 1, 15};
			RTC_set_time_and_date(&time, &date);
			printf("This demo will print the time every 100ms\n");
			break;}
		case RUN:{
			static uint8_t previous_printed_time = 0;
			RTC_TimeTypeDef time;
			RTC_DateTypeDef date;
			RTC_get_time_and_date(&time, &date);
			if(time.Seconds != previous_printed_time)
			{
				printf("%2d/%2d/%2d - %2d:%2d:%2d:%3ld\n", date.Date, date.Month, date.Year, time.Hours, time.Minutes, time.Seconds, time.SubSeconds);
				previous_printed_time = time.Seconds;
			}

			if(ask_for_finish)
			{
				state = INIT;
				ret = END_OK;
			}
			break;}
		default:
			break;
	}
	return ret;
}
char rtctime[20];

running_e RTC_process_main(bool_e ask_for_finish){

	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	running_e ret;
	ret = IN_PROGRESS;

	LCD_SetFont(&Bebasn);

	switch(state)
	{
		case INIT: {
			state = RUN;
			RTC_init(FALSE);	//Oscillateur INTERNE, imprécis, mais testable sans ajout d'un oscillateur sur PC14 et PC15 !
			//RTC_init(TRUE);		//Oscillateur EXTERNE, précis, mais nécessite l'ajout d'un oscillateur sur PC14 et PC15 !

			/*	1/04/2015	12h34'56"
			RTC_TimeTypeDef time = {13, 50, 00, 00, RTC_HOURFORMAT12_AM, RTC_DAYLIGHTSAVING_NONE, RTC_STOREOPERATION_RESET};
			RTC_DateTypeDef date = {RTC_WEEKDAY_THURSDAY, RTC_MONTH_FEBRUARY, 25, 16};
			RTC_set_time_and_date(&time, &date); TODO: Delete init and put set_time function in settings */
			break;}
		case RUN:{
			static uint8_t previous_printed_time = 0;
			RTC_TimeTypeDef time;
			RTC_DateTypeDef date;
			RTC_get_time_and_date(&time, &date);
			if(time.Seconds != previous_printed_time)
			{
				sprintf(rtctime,"%02d:%02d",time.Hours, time.Minutes);
				LCD_DisplayStringLine(226,265,rtctime,0xffff,0x2104,LCD_NO_DISPLAY_ON_UART);
				previous_printed_time = time.Seconds;
			}

			if(ask_for_finish)
			{
				state = INIT;
				ret = END_OK;
			}
			break;}
		default:
			break;
	}

   /* Return the pointer to the dynamically allocated buffer */
   return ret;
}


//______________________________________________



static RTC_HandleTypeDef rtc_handle;


/**
 * @brief initialise la RTC
 * @func void RTC_init(bool_e i_installed_an_external_oscillator)
 * @param i_installed_an_external_oscillator :
 * 					pour utiliser la RTC, vous devez relier un quartz de 32768Hz entre PC14 et PC15.
 * 						Si c'est fait, indiquez TRUE
 * 					Sinon, il est possible d'utiliser l'oscillateur interne : indiquez FALSE
 * 					Attention, cet oscillateur est médiocre (pourcentage de déviation de plusieurs pourcents !)
 * 						mais il est pratique pour valider le concept...
 */
void RTC_init(bool_e i_installed_an_external_oscillator)
{
	__HAL_RCC_PWR_CLK_ENABLE();						//Enable the Power Controller (PWR) APB1 interface clock
	HAL_PWR_EnableBkUpAccess();						//Enable access to RTC domain
	if(i_installed_an_external_oscillator)
	{
		uint32_t tickstart;
		__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);

		tickstart = HAL_GetTick();
		while(!__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY))
		{
			if((HAL_GetTick() - tickstart ) > LSE_TIMEOUT_VALUE)
			{
				printf("LSE is not ready : did you place a 32768 Hz quartz on PC14-PC15?\n");
				return;
			}
		}
		printf("LSE is ready\n");
		__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);	//Select the external RTC clock source
	}
	else
	{
		__HAL_RCC_LSI_ENABLE();
		while(!__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY));
		__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);	//Select the internal RTC clock source
	}
	__HAL_RCC_RTC_ENABLE();							//Enable RTC Clock

	rtc_handle.Instance = RTC;
	rtc_handle.Init.HourFormat = 		RTC_HOURFORMAT_24;
	rtc_handle.Init.AsynchPrediv = 		128-1;		//128*256 = 32768 => 1Hz !
	rtc_handle.Init.SynchPrediv = 		256-1;
	rtc_handle.Init.OutPut = 			RTC_OUTPUT_DISABLE;
	rtc_handle.Init.OutPutPolarity = 	RTC_OUTPUT_POLARITY_HIGH;
	rtc_handle.Init.OutPutType = 		RTC_OUTPUT_TYPE_PUSHPULL;
	HAL_RTC_Init(&rtc_handle);								//Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour
}


void RTC_set_time_and_date(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	HAL_RTC_SetTime(&rtc_handle, sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&rtc_handle, sDate, RTC_FORMAT_BIN);
}

void RTC_get_time_and_date(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	HAL_RTC_GetTime(&rtc_handle, sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&rtc_handle, sDate, RTC_FORMAT_BIN);
}

void RTC_set_alarm(RTC_AlarmTypeDef *sAlarm)
{
	HAL_RTC_SetAlarm(&rtc_handle, sAlarm, RTC_FORMAT_BIN);
}

void RTC_get_alarm(RTC_AlarmTypeDef *sAlarm, uint32_t Alarm)
{
	HAL_RTC_GetAlarm(&rtc_handle, sAlarm, Alarm, RTC_FORMAT_BIN);
}


