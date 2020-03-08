#ifndef __ETHERNET_H__
#define __ETHERNET_H__
#include "stm32f4xx_hal.h"

#define POLLINGMODE		0
#define INTERRUPTMODE	1
 
#define DHCP		0x01
#define NTP			0x02
#define IRQMODE   0x04
#define POLLMODE 	0x00
 
// berechnet die Broadcast-Adresse bei gegebener IP-Adresse und Netzmaske
#define CALC_BROADCAST_ADDR( ip, mask ) ( ip | ~mask )

// Testet, ob eine Adresse die Broadcast-Adresse ist (zu einer Netzwerkmaske)
#define IS_BROADCAST_ADDR( ip, mask ) ( ( ip & ~mask ) == ~mask)

// Schaut ob Ziel-IP in diesen Subnet liegt 
#define IS_ADDR_IN_MY_SUBNET( ip, mask ) ( ( ip & ~mask ) == ( mlIP & ~mask ) )

#define htons(A) ((((A) & 0xff00) >> 8) | (((A) & 0x00ff) << 8))
#define htonl(A) ((((A) & 0xff000000) >> 24) | (((A) & 0x00ff0000) >> 8) | \
			 (((A) & 0x0000ff00) << 8) | (((A) & 0x000000ff) << 24)) 
#define ntohs htons 
#define ntohl htonl


#define ETHERNET_MIN_PACKET_LENGTH	0x3C
 
//IP Protocol Types
#define	PROT_ICMP				0x01	//zeigt an die Nutzlasten enthalten das ICMP Prot
#define	PROT_TCP				0x06	//zeigt an die Nutzlasten enthalten das TCP Prot.
#define	PROT_UDP				0x11	//zeigt an die Nutzlasten enthalten das UDP Prot.	

#define ETH_HDR_LEN 			0x0e	// = 14
#define ARP_HDR_LEN 			0x1c	// = 28
#define IP_HDR_LEN 				0x14	// = 20
#define TCP_HDR_LEN 			0x14	// = 20
#define UDP_HDR_LEN				0x08	// = 8
#define TCP_DATA_START			(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN)
#define UDP_DATA_START			(ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN)

//#define ETHER_OFFSET			0x00
//#define ARP_OFFSET				0x0E
//#define IP_OFFSET				0x0E
//#define UDP_OFFSET				0x22

#define ARP_REPLY_LEN			60
#define ARP_REQUEST_LEN			42
#define ARP_MAX_ENTRY_TIME 		100 //100sec.

#define MAX_UDP_ENTRY			5		// max possible number of udp connections at a time
#define MAX_ARP_ENTRY			5
	
#define TFTP_SERVER_PORT		69

struct ETH_header {
	uint8_t ETH_destMac[6];	
	uint8_t ETH_sourceMac[6];
	uint16_t ETH_typefield;		//Nutzlast 0x0800=IP Datagramm;0x0806 = ARP
};

//----------------------------------------------------------------------------
//Aufbau eines ARP Header
//	
//	2 BYTE Hardware Typ					|	2 BYTE Protokoll Typ	
//	1 BYTE Länge Hardwareadresse (MAC)	|	1 BYTE Länge Protokolladresse (IP)
//	2 BYTE Operation
//	6 BYTE MAC Adresse Absender			|	4 BYTE IP Adresse Absender
//	6 BYTE MAC Adresse Empfänger		|	4 BYTE IP Adresse Empfänger	
struct __attribute__((__packed__))ARP_header {
	uint16_t HWtype;				// 2 Byte
	uint16_t Protocoltype;			// 2 Byte
	uint8_t HWsize;				// 1 Byte
	uint8_t Protocolsize;			// 1 Byte
	uint16_t ARP_Opcode;			// 2 Byte
	uint8_t ARP_sourceMac[6];		// 6 Byte
	uint32_t ARP_sourceIP;			// 4 Byte
	uint8_t ARP_destMac[6];		// 6 Byte
	uint32_t ARP_destIP;			// 4 Byte = 28
};

//----------------------------------------------------------------------------
//Aufbau eines IP Datagramms (B=BIT)
//	
//4B Version	|4B Headergr.	|8B Tos	|16B Gesamtlänge in Bytes	
//16B Identifikation			|3B Schalter	|13B Fragmentierungsposition
//8B Time to Live	|8B Protokoll	|16B Header Prüfsumme 
//32B IP Quelladresse
//32B IB Zieladresse
struct IP_header	{
	uint8_t	IP_Version_Headerlen;	//4 BIT Die Versionsnummer von IP, 
											//meistens also 4 + 4Bit Headergröße 	
	uint8_t	IP_Tos;					//Type of Service
	uint16_t	IP_Totallenght;			//16 Bit Komplette Läng des IP Datagrams in Bytes
	uint16_t	IP_Id;					//ID des Packet für Fragmentierung oder 
											//Reassemblierung
	uint8_t	IP_Flags;
	uint8_t	IP_Frag_Offset;			//wird benutzt um ein fragmentiertes 
											//IP Packet wieder korrekt zusammenzusetzen
	uint8_t	IP_ttl;					//8 Bit Time to Live die lebenszeit eines Paket
	uint8_t	IP_Proto;				//Zeigt das höherschichtige Protokoll an 
											//(TCP, UDP, ICMP)
	uint16_t	IP_Hdr_Cksum;			//Checksumme des IP Headers
	uint32_t	IP_Srcaddr;				//32 Bit IP Quelladresse
	uint32_t	IP_Destaddr;			//32 Bit IP Zieladresse
};

#define TFTP_FILENAME_SIZE 20
#define TFTP_TYPE_SIZE 20

extern uint32_t mlIP;
extern uint32_t mlNetmask;
extern uint32_t mlGateway;
extern uint32_t mlDNSserver;
extern uint8_t mlMAC[];
extern uint8_t ethernetbuffer[];
extern uint32_t mlTFTPip;
extern uint8_t msTFTPfileName[TFTP_FILENAME_SIZE];

void stack_init (void) ;

void eth_packet_dispatcher(void) ;

void Make_ETH_Header (uint8_t *buffer, uint32_t dest_ip) ;

void Make_IP_Header (uint8_t *buffer, uint32_t dest_ip) ;


#endif
