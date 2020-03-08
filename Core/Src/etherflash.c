#include <stdlib.h>
#include "config.h"
#include "eemem.h"
#include "enc28j60.h"
#include "ethernet.h"
#include "arp.h"
#include "udp.h"
#include "etherflash.h"
#include "checksum.h"
#include <flash_if.h>
#include "console.h"   
#include "string.h"   
#include "stdio.h"   


uint8_t lineBuffer[600];
uint16_t rxBufferIdx,len;
uint8_t starApp = 0;
uint32_t baseAddress = 0;
uint16_t bytesInBootPage;
uint32_t currentAddress;
struct UDP_SOCKET sock;
uint16_t tftpTimeoutCounter;

static uint8_t mymac[6] = {MYMAC1,MYMAC2,MYMAC3,MYMAC4,MYMAC5,MYMAC6};
uint8_t rev = 0;

uint8_t flagOACK = 0;
uint8_t sendReq = 0;

// local functions
void sendTFTPrequest(void);
void Start_Application(void);

typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;

void pConsoleCallbackHandle(uint8_t *data, uint8_t dataLen);

typedef enum 
{
	teMsgHelp = 0x0,
	teMsgInitEnc,
	teMsgDeleteFlash,
	teMsgLoadFlash,
	teMsgRunUserCode,
	teMsgReset,
	teMsgLast,	
}teMsgList;

char msgArray [6][1] = {
	'?',
	'i',
	'd',
	'l',
	'r',
	'm'
};

void etherflash_main(void)
{
	
	enc28j60Init( mymac );
	rev = enc28j60getrev();
						
	stack_init();
	pConsoleInit(pConsoleCallbackHandle);
	consoletTxHandle("STM32 BOOTLOADER \r\n",strlen("STM32 BOOTLOADER \r\n"));
		
	BootLoaderMain();	
}

void BootLoaderMain(void)
{
	uint8_t nRetryCounter = 0;

	// init global vars
	baseAddress = 0;
	bytesInBootPage = 0;
	currentAddress = 0;
	tftpTimeoutCounter = 0;
	
	sock.DestinationIP = 0;
	sock.Bufferfill = 0;
	sock.BlockNumber = 0;
	sock.lineBufferIdx = 0;
	sock.SourcePort = ~TFTP_SERVER_PORT;

	// send initial TFTP RRQ
  UDP_RegisterSocket (sock.SourcePort, (void(*)(void))tftp_get);
	/*HAL_Delay(5000);
	sendTFTPrequest();*/
	
	while (1)
	{
		eth_packet_dispatcher();
	
		HAL_Delay(2);
		if(sendReq)
		{
			sendTFTPrequest();
			sendReq=0;
		}
		if(starApp)
		{
			Start_Application();
			starApp = 0;
		}
		/*if (tftpTimeoutCounter++ > 100)
		{
			if ((sock.DestinationIP != 0) && (nRetryCounter++ < 4))
			{	// ok, we had contact to a server, may be it was
				// the first contact to discover the ip.
				// Try again, but not unlimited
				tftpTimeoutCounter = 0;
				//sendTFTPrequest();
			}
		}*/
	}
}

void sendTFTPrequest(void)
{
	uint8_t *udpSendBuffer;
	uint8_t reqSize;
	uint8_t *file;

	udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);

	file = msTFTPfileName;

	reqSize = 0;

	//opcode read request
	*udpSendBuffer++ = 0x00; 
	*udpSendBuffer++ = 0x01;
	reqSize += 2;
	//source file name
	while (*file)
	{
		*udpSendBuffer++ = *file++;
		reqSize++;
	}
	*udpSendBuffer++ = 0x00;
	//type octet
	*udpSendBuffer++ = 'o';
	*udpSendBuffer++ = 'c';
	*udpSendBuffer++ = 't';
	*udpSendBuffer++ = 'e';
	*udpSendBuffer++ = 't';
	*udpSendBuffer++ = 0x00;
	reqSize += 7;
	//option name : timeout	
	*udpSendBuffer++ = 0x74;
	*udpSendBuffer++ = 0x69;
	*udpSendBuffer++ = 0x6d;
	*udpSendBuffer++ = 0x65;
	*udpSendBuffer++ = 0x6f;
	*udpSendBuffer++ = 0x75;
	*udpSendBuffer++ = 0x74;
	*udpSendBuffer++ = 0x00;
	reqSize += 8;
	//option value
	*udpSendBuffer++ = 0x35;
	*udpSendBuffer++ = 0x00;
	reqSize += 2;
	//option blk size
	*udpSendBuffer++ = 0x62;
	*udpSendBuffer++ = 0x6c;
	*udpSendBuffer++ = 0x6b;
	*udpSendBuffer++ = 0x73;
	*udpSendBuffer++ = 0x69;
	*udpSendBuffer++ = 0x7a;
	*udpSendBuffer++ = 0x65;
	*udpSendBuffer++ = 0x00;
	reqSize += 8;
	//option value : 512
	*udpSendBuffer++ = 0x35;
	*udpSendBuffer++ = 0x31;
	*udpSendBuffer++ = 0x32;
	*udpSendBuffer++ = 0x00;
	reqSize += 4;

	//reqSize += 22;
	//mlTFTPip = IP(192,168,1,10);
	mlTFTPip = IP(192,168,2,168);
	sock.DestinationIP = mlTFTPip;

	UDP_SendPacket (reqSize, sock.SourcePort, TFTP_PORT, mlTFTPip);		
}

void tftp_get (void)
{
	struct ETH_header * ETH_packet; 		// ETH_struct anlegen
	struct IP_header * IP_packet;		// IP_struct anlegen
	struct UDP_header * UDP_packet;
	struct TFTP_RESPONSE *tftp;
			
	char string[50];

	uint8_t *udpSendBuffer;
	uint8_t reqSize;
	uint8_t *file;
	uint32_t sourceAdress;
	uint32_t newbaseAddress;
	uint16_t packetLen = 0;
	udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);

	ETH_packet = (struct ETH_header *) ethernetbuffer;
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETH_HDR_LEN];
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];
	tftp = (struct TFTP_RESPONSE *)&ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HDR_LEN];

	uint8_t lastPacket = 0;

	// Reset timeout counter
	tftpTimeoutCounter = 0;
	
	if (sock.DestinationIP != IP_packet->IP_Srcaddr)
	{	// other TFTP-Server is sending data - ignore it.
		return;
	}
	
	sock.DestinationPort = htons(UDP_packet->UDP_SourcePort);
	sock.BlockNumber = htons(tftp->blockNumber);
	sock.Bufferfill = htons(UDP_packet->UDP_Datalenght) - UDP_HDR_LEN;
	
	len = htons(UDP_packet->UDP_Datalenght);
	
	if (tftp->op == TFTP_OP_OACK)
	{			
		uint8_t *udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
		tftp->op = 0x0400;
		tftp->blockNumber = 0x0000;
		UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
	}
	else if (tftp->op == TFTP_OP_DATA)
	{
			// last packet is shorter than 516 bytes
			if (sock.Bufferfill < 516) // 512 + 4 
			{
					lastPacket = 1;
			}
		
			if(lastPacket == 1)
			{
				if((sock.Bufferfill - 4) == 0)
				{
					//there is no data on the last packet
					tftp->op = 0x0400;
					// mark buffer free
					sock.Bufferfill = 0;
					UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
				}
				else
				{

					packetLen = (sock.Bufferfill - 4) ;
					
					if((packetLen % 4) == 0)
					{//kalan paket 4 un tam kat1 ise
						sourceAdress = (uint32_t)&ethernetbuffer[(ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN) + 4 ];
						newbaseAddress = USER_START_ADDRESS + (512 * baseAddress);
						FLASH_If_Write(newbaseAddress, (uint32_t*)sourceAdress, packetLen / 4);
					}
					else
					{
						newbaseAddress = USER_START_ADDRESS + (512 * baseAddress);
						sourceAdress = (uint32_t)&ethernetbuffer[(ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN) + 4 ];
						FLASH_If_Write(newbaseAddress, (uint32_t*)sourceAdress, (packetLen / 4) + 1);
					}
					
					tftp->op = 0x0400;
					sock.Bufferfill = 0;
					UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
				}
				
				UDP_UnRegisterSocket(sock.SourcePort);
				sprintf(string,"\r\nLOAD FLASH COMPLETED\r\n");
				consoletTxHandle(string,strlen(string));	
				Start_Application();
			}
			else
			{
				newbaseAddress = USER_START_ADDRESS + (512 * baseAddress);
				sourceAdress = (uint32_t)&ethernetbuffer[(ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN) + 4];
				if(FLASH_If_Write(newbaseAddress, (uint32_t*)sourceAdress, (sock.Bufferfill - 4) / 4) != FLASHIF_OK)
				{
						UDP_UnRegisterSocket(sock.SourcePort);
				}
				tftp->op = 0x0400;
				sock.Bufferfill = 0;
				baseAddress++;
				UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
				sprintf(string,"#");
				consoletTxHandle(string,strlen(string));	
			}		
	}
	else if (tftp->op == TFTP_OP_ERR)
	{
		// error -> reboot to application
		UDP_UnRegisterSocket(sock.SourcePort);
		Start_Application();
	}
}

void Start_Application(void)
{
	char string[50];
	
	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	if (((*(__IO uint32_t*) USER_START_ADDRESS ) & 0x2FFE0000) == 0x20000000) {
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (USER_START_ADDRESS + 4);
		JumpToApplication = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) USER_START_ADDRESS);
		JumpToApplication();
	}
	else 
	{
		sprintf(string,"there is no user code !!! \r\n");
		consoletTxHandle(string,strlen(string));		
		//NVIC_SystemReset();
	}
}

void pConsoleCallbackHandle(uint8_t *data, uint8_t dataLen)
{
		uint8_t cnt = 0;
		char string[50];
	
		if(strncmp((char*)data,&msgArray[teMsgHelp][0],1)==0)
		{
			sprintf(string,"HELP \r\n");
			consoletTxHandle(string,strlen(string));	
			for(cnt = 0; cnt < teMsgLast ; cnt++)
			{
					sprintf(string,"%c\r\n",msgArray[cnt][0]);
					consoletTxHandle(string,strlen(string));
			}
		}
		else if(strncmp((char*)data,&msgArray[teMsgInitEnc][0],1)==0)
		{
			sprintf(string,"INIT ENC28 \r\n");
			consoletTxHandle(string,strlen(string));	
				
			sprintf(string,"INIT ENC28 Completed\r\n");
			consoletTxHandle(string,strlen(string));	
		}
		else if(strncmp((char*)data,&msgArray[teMsgDeleteFlash][0],1)==0)
		{
			sprintf(string,"ERASE FLASH \r\n");
			consoletTxHandle(string,strlen(string));	
			FLASH_If_Init();
			FLASH_If_Erase(USER_START_ADDRESS);
			sprintf(string,"ERASE FLASH Completed\r\n");
			consoletTxHandle(string,strlen(string));	
		}
		else if(strncmp((char*)data,&msgArray[teMsgLoadFlash][0],1)==0)
		{
				sprintf(string,"LOAD FLASH \r\n");
				consoletTxHandle(string,strlen(string));	
				sendReq=1;//sendTFTPrequest();
		}
		else if(strncmp((char*)data,&msgArray[teMsgRunUserCode][0],1)==0)
		{
				sprintf(string,"STAR USER FLASH \r\n");
				consoletTxHandle(string,strlen(string));
				starApp = 1;
		}
		else if(strncmp((char*)data,&msgArray[teMsgReset][0],1)==0)
		{
				sprintf(string,"RESET \r\n");
				consoletTxHandle(string,strlen(string));
				NVIC_SystemReset();
		}
}

