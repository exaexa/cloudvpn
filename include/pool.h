
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

#ifndef _CVPN_CHAIN_H
#define _CVPN_CHAIN_H

#include <stdint.h>

#include "plugin.h"

/*
 * part is an instance of plugin
 */

struct part {
	uint32_t id;
	struct plugin*p;
	void*data;
	char*id;
};

/*
 * pool is a set of parts
 */

#endif
