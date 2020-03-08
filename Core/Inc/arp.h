#ifndef __ARP_H__
#define __ARP_H__

#include "main.h"

struct ARP_TABLE {
	uint32_t IP;
	uint8_t MAC[6];
	uint8_t time;
};

void arp_reply (void) ;
void arp_entry_add (uint32_t sourceIP, uint8_t *sourceMac) ;
unsigned char *arp_entry_search (uint32_t dest_ip);

#endif
