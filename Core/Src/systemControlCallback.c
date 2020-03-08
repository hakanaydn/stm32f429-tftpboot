#include <systemControlCallback.h>
#include <main.h>
#include <enc28j60.h>
#include <console.h>
#include <string.h>
#include <stdio.h>
#include <etherflash.h>

#define ONE_SECOND 1
#define ONE_MINUTE ONE_SECOND * 60

static void pPortControlCallback(void);
static void pAlarmAndLedControlCallback(void);
static void pPressAnyKeyControlCallback(void);

extern TIM_HandleTypeDef htim11;
extern uint8_t starApp ;

uint8_t linkStatus = 2;
extern uint8_t consolePressAnyKey;

tsSystemControl sSystemControl [] =
{
	{ePortControl,teControl_active,ONE_SECOND,ONE_SECOND,pPortControlCallback},
	{eAlarmLedControl,teControl_passive,ONE_SECOND,ONE_SECOND,pAlarmAndLedControlCallback},
	{eConsolPress,teControl_active,ONE_SECOND,ONE_SECOND,pPressAnyKeyControlCallback},
	{eLastControl,teControl_passive,0,0,NULL}
};

static void pPortControlCallback(void)
{
	linkStatus = enc28j60linkup();
}

static void pAlarmAndLedControlCallback(void)
{
	
}

void pSystemControlCallback(void)
{
	for(uint8_t cnt = 0; cnt < eLastControl ; cnt++)
	{
		if(sSystemControl[cnt].status == teControl_active)
		{
			sSystemControl[cnt].leftTime--;
			if(sSystemControl[cnt].leftTime == 0)
			{
				sSystemControl[cnt].pfn_controlCallbackFunc();
				sSystemControl[cnt].leftTime = sSystemControl[cnt].periodTime;
			}
		}
	}

}
static void pPressAnyKeyControlCallback(void)
{
	static uint8_t cnt = 0;
	const  uint8_t timeout = 15;	
	char string[50];
	if(consolePressAnyKey == 1)
	{
			sSystemControl[eConsolPress].status = teControl_passive;
	}
	else
	{
			cnt++;
			sprintf(string,"Press Any Key Left Time :%d sn\r\n",cnt);
			consoletTxHandle(string,strlen(string));
			if(cnt == timeout)
			{
				 sSystemControl[eConsolPress].status = teControl_passive;
				 sprintf(string,"Goto User Space\r\n");
				 consoletTxHandle(string,strlen(string));
				 starApp = 1;
			}
	}
}

void pSystemControlCallbackStart(void)
{
	HAL_TIM_Base_Start_IT(&htim11);
}

