#include <stdint.h>
#include "config.h"
#include "checksum.h"
#include "ethernet.h"

uint16_t Checksum_16 (uint8_t * pointer, uint16_t headerlenght, uint32_t startval)
{
	struct TPWORDACC
	{
		uint16_t DataL;
		uint16_t DataH;
	};
	union
	{
		struct TPWORDACC nWordAcc;
		uint32_t lLongAcc;
	} checksum;
	checksum.lLongAcc = htons(startval);
	uint16_t *pP16 = (uint16_t *)pointer;
	
	//Jetzt werden alle Packete in einer While Schleife addiert
	while( headerlenght > 0)
	{
		if( headerlenght == 1)
		{	// headerlenght ist ungerade, füge eine null hinzu (DataH einfach weglassen)
			checksum.lLongAcc += (*pP16 & 0x00ff);
			break;
		}
		else
		{
			checksum.lLongAcc += *pP16++;
			headerlenght -=2 ;
		}
	}

	//Komplementbildung (addiert Long INT_H Word mit Long INT L Word)
	while (checksum.nWordAcc.DataH)
		checksum.lLongAcc = (uint32_t)checksum.nWordAcc.DataL + (uint32_t)checksum.nWordAcc.DataH;
	return ~(checksum.nWordAcc.DataL);
}