#include <stdio.h>
#include "config.h"
#include "eemem.h"
#include "ethernet.h"
#include "checksum.h"
#include "arp.h"
#include "enc28j60.h"
#include "udp.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"

uint32_t mlIP;
uint32_t mlNetmask;
uint32_t mlGateway;
uint32_t mlDNSserver;
uint8_t mlMAC[6];
uint8_t ethernetbuffer[MTU_SIZE];
uint32_t mlTFTPip;
uint8_t msTFTPfileName[TFTP_FILENAME_SIZE]=CONFIG_REQ_FILENAME_3SYM;

unsigned int IP_id_counter;

void stack_init (void)
{
	IP_id_counter = 0;
		
	// Read IP Setup from EEPROM
	//mlIP = IP(192,168,1,20);
	mlIP = IP(192,168,2,177);
	mlNetmask = IP(255,255,255,0);
	mlGateway = mlGatewayEEP = IP(0,0,0,0);
	mlDNSserver = mlDNSserverEEP = IP(0,0,0,0);

	mlMAC[ 0 ] = MYMAC1;
	mlMAC[ 1 ] = MYMAC2;
	mlMAC[ 2 ] = MYMAC3;
	mlMAC[ 3 ] = MYMAC4;
	mlMAC[ 4 ] = MYMAC5;
	mlMAC[ 5 ] = MYMAC6;

	udp_init();
}
	  
void eth_packet_dispatcher(void)
{
	uint16_t packet_length;

	// hole ein Frame
	packet_length = enc28j60PacketReceive (MTU_SIZE, ethernetbuffer);

	// wenn Frame vorhanden packet_lenght != 0
	// arbeite so lange die Frames ab bis keine mehr da sind
	if (packet_length != 0)
	{		
		struct ETH_header *ETH_packet; 		//ETH_struc anlegen

		ETH_packet = (struct ETH_header *) ethernetbuffer;

		switch ( ETH_packet->ETH_typefield ) // welcher type ist gesetzt
		{
			case 0x0608:
				arp_reply ();
				break;

			case 0x0008:
				if (((struct IP_header *)&ethernetbuffer[ETH_HDR_LEN])->IP_Proto == PROT_UDP)
				{
					udp();
				}
				break;
		}
	}
}

//----------------------------------------------------------------------------
//Diese Routine Erzeugt ein neuen Ethernetheader
void Make_ETH_Header (uint8_t *buffer, uint32_t dest_ip)
{
	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;

	uint8_t i;
	unsigned char *pDestMac = arp_entry_search(dest_ip);
	for ( i = 0 ; i < 6 ; i++ )
	{
		ETH_packet->ETH_sourceMac[i] = mlMAC[i];
		ETH_packet->ETH_destMac[i] = pDestMac?pDestMac[i]:0xff;
	}

}

//----------------------------------------------------------------------------
//Diese Routine erzeugt ein IP Packet
void Make_IP_Header (uint8_t *buffer, uint32_t dest_ip)
{
    unsigned int result16;  //Checksum
    struct ETH_header *ETH_packet;
    struct IP_header       *ip;

    ETH_packet = (struct ETH_header *)buffer;
    ip       = (struct IP_header *)&buffer[ETH_HDR_LEN];

    Make_ETH_Header (buffer, dest_ip);         //Erzeugt einen neuen Ethernetheader
    ETH_packet->ETH_typefield = 0x0008;	// htons(0x0800) Nutzlast 0x0800=IP

    IP_id_counter++;

    ip->IP_Flags		= 0x40;
    ip->IP_Frag_Offset	= 0x00;  //don't fragment
    ip->IP_ttl			= 64;      //max. hops
    ip->IP_Id			= htons(IP_id_counter);
    ip->IP_Version_Headerlen    = 0x45;  //4 BIT Die Versionsnummer von IP, 
    ip->IP_Tos			= 0;
    ip->IP_Destaddr     = dest_ip;
    ip->IP_Srcaddr		= mlIP;
    ip->IP_Hdr_Cksum	= 0;
  
    //Berechnung der IP Header länge  
    result16 = (ip->IP_Version_Headerlen & 0x0F) << 2;

    //jetzt wird die Checksumme berechnet
    ip->IP_Hdr_Cksum = Checksum_16 (&ip->IP_Version_Headerlen, result16, 0);

    return;
}