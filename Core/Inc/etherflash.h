#ifndef ETHERFLASH_H
#define ETHERFLASH_H
#include "stm32f4xx_hal.h"
#include "ethernet.h"

#pragma anon_unions

struct UDP_SOCKET 
{
	uint32_t DestinationIP;
	uint16_t SourcePort;
	uint16_t DestinationPort;
	uint16_t Bufferfill;
	uint16_t DataStartOffset;
	uint16_t BlockNumber;
	uint16_t lineBufferIdx;
};

struct TFTP_RESPONSE
{
  uint16_t op;		//1		Message op code / message type
		      				//1 = RRQ, 2 = WRQ, 3 = DATA, 4 = ACK, 5 = ERR
	union
	{
		uint16_t blockNumber;
		uint16_t errCode;
	};
	union
	{
		uint8_t *data;
		char *errMsg;
	};
};

#define TFTP_OP_DATA  0x0300
#define TFTP_OP_ERR   0x0500
#define TFTP_OP_OACK  0x0600

#define TFTP_PORT  69


void BootLoaderMain(void);
void tftp_get (void);
uint8_t hexToByte(uint8_t *buf, uint16_t idx);
void etherflash_main(void);
void Start_Application(void);

extern uint8_t starApp ;

#endif
