#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include <arp.h>
#include "ethernet.h"
#include "enc28j60.h"
#include "stdlib.h"
#include "stdio.h"


struct ARP_TABLE ARPtable[ MAX_ARP_ENTRY ];
extern uint8_t ethernetbuffer[MTU_SIZE];
//----------------------------------------------------------------------------
//Diese Routine Antwortet auf ein ARP Paket
struct ETH_header *ETH_packet;
struct ARP_header *ARP_packet;

void arp_reply (void)
{
	ETH_packet = (struct ETH_header *)ethernetbuffer;
	ARP_packet = (struct ARP_header *)&ethernetbuffer[ETH_HDR_LEN];

	if(ARP_packet->ARP_destIP == mlIP) // Für uns?
	{
		uint8_t i;
			if (ARP_packet->ARP_Opcode == 0x0100) // htons(0x0001)   // Request?
			{
					arp_entry_add(ARP_packet->ARP_sourceIP, ARP_packet->ARP_sourceMac); 
					Make_ETH_Header (ethernetbuffer, ARP_packet->ARP_sourceIP); // Erzeugt ein neuen Ethernetheader
					ETH_packet->ETH_typefield = 0x0608; // htons(0x0806) Nutzlast 0x0800=IP Datagramm;0x0806 = ARP
		
					// mac und ip des senders in ziel kopieren
					for ( i = 0; i < 10; i++ )
						ARP_packet->ARP_destMac[i] = ARP_packet->ARP_sourceMac[i]; // MAC und IP umkopieren
					
					// meine mac und ip als absender einsetzen
					for ( i = 0; i < 6 ; i++ )
						ARP_packet->ARP_sourceMac[i] = mlMAC[i];
					
					ARP_packet->ARP_sourceIP = mlIP; // IP einsetzen
		
					ARP_packet->ARP_Opcode = 0x0200;	// htons(0x0002) ARP op = ECHO  
		
					enc28j60PacketSend(ARP_REPLY_LEN, ethernetbuffer);          // ARP Reply senden...
					return;
			}
			else if ( ARP_packet->ARP_Opcode == 0x0200 )  // htons(0x0002) REPLY von einem anderen Client
			{
					arp_entry_add(ARP_packet->ARP_sourceIP, ARP_packet->ARP_sourceMac);
			}
	}
  return;
}

//----------------------------------------------------------------------------
//erzeugt einen ARP - Eintrag wenn noch nicht vorhanden 
void arp_entry_add (uint32_t sourceIP, uint8_t *sourceMac)
{
    //Eintrag schon vorhanden?
		uint8_t i;
    for (i = 0; i < MAX_ARP_ENTRY; i++)
    {
			if(ARPtable[i].IP == sourceIP)
			{
				//Eintrag gefunden Time refresh
				ARPtable[i].time = ARP_MAX_ENTRY_TIME;
				return;
			}
    }
  
    //Freien Eintrag finden
    for (i = 0; i < MAX_ARP_ENTRY; i++)
    {
			if(ARPtable[i].IP == 0)
			{
				for(uint8_t a = 0; a < 6; a++)
				{
					ARPtable[i].MAC[a] = sourceMac[a]; 
				}
				ARPtable[i].IP   = sourceIP;
				ARPtable[i].time = ARP_MAX_ENTRY_TIME;
				return;
			}
    }
    return;
}


//----------------------------------------------------------------------------
//Diese Routine sucht anhand der IP den ARP eintrag
unsigned char *arp_entry_search (uint32_t dest_ip)
{
	for (uint8_t i = 0; i < MAX_ARP_ENTRY; i++)
	{
		if(ARPtable[i].IP == dest_ip)
		{
			return ARPtable[i].MAC;
		}
	}
	return NULL;
}
