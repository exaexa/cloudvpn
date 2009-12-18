

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

#ifndef _CVPN_ROUTETREE_H
#define _CVPN_ROUTETREE_H

#include <stdint.h>

struct routeinfo_t {
	uint32_t ping;
	uint32_t dist;
	int id;
};

struct rtentry_t {
	uint8_t d, c, al_c;
	struct rtentry_t*p;
	struct routeinfo_t*ri;
};

void rtentry_init (rtentry_t**);
void rtentry_free (rtentry_t**);


#endif
