 /**
 ******************************************************************************
 * @file      console.c
 * @authors   Hakan AYDIN
 * @date      04.10.2019
 * @version   V0.0.1
 * @copyright Copyright (c) 2019
 * @brief     console Source File.
 ******************************************************************************
 */

/*****************************************************************************/
/*                           INCLUDES	                                     */
/*****************************************************************************/
#include "stm32f4xx_hal.h"
#include "console.h"   
#include "systemControlCallback.h"   

/*****************************************************************************/
/*                           EXTERN                                          */
/*****************************************************************************/
extern UART_HandleTypeDef huart1;
extern tsSystemControl sSystemControl;
uint8_t consolePressAnyKey = 0;

/*****************************************************************************/
/*                          TYPEDEF 				                                 */
/*****************************************************************************/
typedef struct{
	uint8_t CounterCharacters;
	char Characters[50];/*CONSOLE ARRAY OVERRITE EKLE ALARM OLARAK*/
	char rxCharacter;
	void (*pfn_consoleHandle)(uint8_t *data, uint8_t dataLen);
}tsRxConsole;


tsRxConsole  sRxConsole;
/*****************************************************************************/
/*                           FUNCTIONS                                       */
/*****************************************************************************/
/**
 * @brief   Console IRQ Init
 * @param   None
 * @retval  None
 */
void pConsoleInit(void(*pfn)(uint8_t *data, uint8_t dataLen))
{
			HAL_UART_Receive_IT(&huart1,(uint8_t*)&sRxConsole.rxCharacter, 1);	
			sRxConsole.CounterCharacters = 0;
			sRxConsole.pfn_consoleHandle = *pfn;
}

/**
 * @brief   IRQ HANDLER USART -- CONSOLDAN DATA GELDIGINDE BURAYA DUSER
 * @param   None
 * @retval  None
 */
 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) 
	{
		if(sRxConsole.rxCharacter != 13/*ENTER TUSU*/)
		{					
					/*ENTER TUSU BASILANA KADAR GELEN KARAKTERLERI KAYDET
					EGER KARAKTER SAYISI FAZLA ISE UYARI VER
					*/
					sRxConsole.Characters[sRxConsole.CounterCharacters] = sRxConsole.rxCharacter;
					sRxConsole.CounterCharacters++;
					consolePressAnyKey = 1;
		}
		else
		{
					sRxConsole.pfn_consoleHandle((uint8_t*)&sRxConsole.Characters,sRxConsole.CounterCharacters);
					sRxConsole.CounterCharacters = 0;
		}
		HAL_UART_Receive_IT(&huart1,(uint8_t*)&sRxConsole.rxCharacter, 1);	
	}
}

/**
 * @brief   Consoldan gelen paket burada degerlendirilir
 * @param   None
 * @retval  None
 */
 
void consoletTxHandle(void *data, uint8_t dataLen)
{
		HAL_UART_Transmit(&huart1,(uint8_t*)data,dataLen,10);
	  sRxConsole.CounterCharacters = 0;
}

