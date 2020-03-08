/*!\file udp.h \brief Definitionen fuer UDP */
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _UDP_H
#define _UDP_H

#include "stm32f4xx_hal.h"


	typedef struct
	{
		uint16_t port;		// Port
		void(*fp)(void);  	// Zeiger auf auszuführende Funktion
	} UDP_PORT_ITEM;

	/*!\struct UDP_header
	 * \brief Definiert den UDP_header.
	 */
	struct UDP_header {
		uint16_t UDP_SourcePort;
		uint16_t UDP_DestinationPort;
		uint16_t UDP_Datalenght;
		uint16_t UDP_Checksum;
	};

	
	void udp_init(void);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*! Hier findet die Bearbeitung des Packetes statt welches ein UDP-Packet enthaelt. Es wir versucht die
	 * Verbindung zuzuordnen, wenn dies nicht moeglich ist wird hir abgebrochen.
	 * Danach wird der Inhalt dem Socket zugeordnet und Daten in den Puffer des Benutzer kopiert.
	 * \warning Zu lange UDP-Packete werden abgeschnitten.
	 * \param 	packet_lenght	Gibt die Packetgroesse in Byte an die das Packet lang ist.
	 * \param	ethernetbuffer	Zeiger auf das Packet.
	 * \return  NONE
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	void udp (void);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Sendet ein UDP-Packet an einen Host.
	 * \param 	SOCKET			Die Socketnummer ueber die das Packet gesendet wird.
	 * param	Datalenght		Gibt die Datenlaenge der Daten in Byte an die gesendet werden sollen.
	 * \param	UDP_Databuffer  Zeifer auf den Datenpuffer der gesendet werden soll.
	 * \return  Bei einem Fehler beim versenden wird ungleich 0 zurueckgegeben, sonst 0.
	 * \sa UDP_RegisterSocket , UDP_GetSocketState
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
void UDP_SendPacket(uint16_t data_length, 
					uint16_t src_port, 
					uint16_t dest_port, 
					uint32_t dest_ip);

uint8_t UDP_RegisterSocket (uint16_t port, void(*fp1)(void));
void UDP_UnRegisterSocket (uint16_t port);

	

#endif /* _UDP_H */
