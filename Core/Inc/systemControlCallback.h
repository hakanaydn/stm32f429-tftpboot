#ifndef SYSTEMCONTROLCALLBACK_H
#define SYSTEMCONTROLCALLBACK_H
#include "stm32f4xx_hal.h"

typedef enum 
{
	ePortControl,
	eAlarmLedControl,
	eConsolPress,
	eLastControl
}teControlItems;

typedef enum 
{
	teControl_active = 0x0,
	teControl_passive,
}teControlStatus;

typedef struct 
{
	teControlItems Item;
	teControlStatus status;
	uint16_t  periodTime;
	uint16_t  leftTime;
	void (*pfn_controlCallbackFunc)(void);
}tsSystemControl;


void pSystemControlCallback(void);
void pSystemControlCallbackStart(void);

#endif /* SYSTEMCONTROLCALLBACK_H */
