#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "checksum.h"
#include "ethernet.h"
#include "udp.h"
#include "arp.h"
#include "enc28j60.h"

UDP_PORT_ITEM UDP_PORT_TABLE[MAX_UDP_ENTRY]; 

void udp_init(void)
{
	for (uint8_t i=0; i<MAX_UDP_ENTRY; i++)
	{
			UDP_PORT_TABLE[i].port = 0;
	}
}

void udp (void)
{
	uint8_t port_index = 0;	

	struct IP_header * IP_packet;		
	struct UDP_header * UDP_packet;

	IP_packet = ( struct IP_header *) &ethernetbuffer[ETH_HDR_LEN];
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];
	
	while (UDP_PORT_TABLE[port_index].port && UDP_PORT_TABLE[port_index].port != htons(UDP_packet->UDP_DestinationPort))
	{ 
		port_index++;
	}
	
	if (!UDP_PORT_TABLE[port_index].port)
	{ 	
		return;
	}

	UDP_PORT_TABLE[port_index].fp(); 
}

uint8_t UDP_RegisterSocket (uint16_t port, void(*fp1)(void))
{
	uint8_t port_index = 0;
	while ((UDP_PORT_TABLE[port_index].port) && (port_index < MAX_UDP_ENTRY))
	{ 
		port_index++;
	}
	if (port_index >= MAX_UDP_ENTRY)
	{
		return 0;
	}
	UDP_PORT_TABLE[port_index].port = port;
	UDP_PORT_TABLE[port_index].fp = *fp1;
	return 1;
}

void UDP_UnRegisterSocket (uint16_t port)
{
	  uint8_t i;

    for (i = 0; i < MAX_UDP_ENTRY; i++)
    {
        if ( UDP_PORT_TABLE[i].port == port )
        {
            UDP_PORT_TABLE[i].port = 0;
        }
    }
}

void UDP_SendPacket(uint16_t data_length, uint16_t src_port, uint16_t dest_port, uint32_t dest_ip)
{
    uint16_t  result16;
    uint32_t result32;

    struct UDP_header *udp;
    struct IP_header  *ip;
	
    udp = (struct UDP_header *)&ethernetbuffer[ETH_HDR_LEN + IP_HDR_LEN];
    ip  = (struct IP_header  *)&ethernetbuffer[ETH_HDR_LEN];
  
    udp->UDP_SourcePort  = htons(src_port);
    udp->UDP_DestinationPort = htons(dest_port);

    data_length     += UDP_HDR_LEN;                //UDP Packetlength
    udp->UDP_Datalenght = htons(data_length);

    data_length     += IP_HDR_LEN;                //IP Headerlänge
    ip->IP_Totallenght = htons(data_length);
	
    data_length += ETH_HDR_LEN; // total packet len
	
    ip->IP_Proto = PROT_UDP;
    Make_IP_Header (ethernetbuffer,dest_ip);

    udp->UDP_Checksum = 0;
  
    //Berechnet Headerlänge und Addiert Pseudoheaderlänge 2XIP = 8
    result16 = htons(ip->IP_Totallenght) + 8;
    result16 = result16 - ((ip->IP_Version_Headerlen & 0x0F) << 2);
	  // will never exceed 0xffff, so word arithmetic should do
    result32 = result16 + 0x09;
  
    //Routine berechnet die Checksumme
    result16 = Checksum_16 ((&ip->IP_Version_Headerlen+12), result16, result32);
    udp->UDP_Checksum = result16;
		
		enc28j60PacketSend(data_length,ethernetbuffer); 
}
