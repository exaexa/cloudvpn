
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

#ifndef _CVPN_ROUTE_H
#define _CVPN_ROUTE_H

#include "comm.h"
#include "address.h"

#include <stdint.h>
#include <stddef.h>

#include <map>
using std::map;

void route_init();
void route_shutdown();
void route_update();
void route_periodic_update();

uint32_t new_packet_uid();
uint16_t new_packet_ttl();

#define route_new_packet(a...) \
route_packet(new_packet_uid(), new_packet_ttl(), ##a)

void route_packet (
    uint32_t id, uint16_t ttl, uint32_t inst,
    uint16_t dof, uint16_t ds,
    uint16_t sof, uint16_t ss,
    uint16_t s, const uint8_t*buf, int from);


void route_set_dirty();
void route_report_to_connection (connection&c);

#endif

