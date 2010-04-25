
/*
 * CloudVPN
 *
 * This program is a free software: You can redistribute and/or modify it
 * under the terms of GNU GPLv3 license, or any later version of the license.
 * The program is distributed in a good hope it will be useful, but without
 * any warranty - see the aforementioned license for more details.
 * You should have received a copy of the license along with this program;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CVPN_PACKET_H
#define _CVPN_PACKET_H

#include <stdint.h>

/*
 * packet allocation functions.
 *
 * packet data are forced to contain:
 *
 * -packet destination address part (offset 0)
 * -packet source address part (offset soff)
 * -packet payload (offset doff)
 *
 * -in_part is the part where the packet came from
 * -next_part is where the packet is supposed to go next
 * -mark is a voluntarily filled-in integer that everyone can fiddle with
 */

struct packet {
	char*data;
	uint16_t len; //total length of datagram data
	uint16_t soff;
	uint16_t doff;

	uint32_t in_part, out_part, next_part, mark;
};

struct packet* cloudvpn_packet_alloc();
void cloudvpn_packet_free (struct packet*);

int cloudvpn_alloc_data (struct packet*);


#endif

