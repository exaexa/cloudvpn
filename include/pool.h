
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

#ifndef _CVPN_POOL_H
#define _CVPN_POOL_H

#include <stdint.h>

struct part;

#include "plugin.h"
#include "mutex.h"

/*
 * part is an instance of plugin
 * pool is a set of parts
 */

struct part {
	uint32_t id;
	struct plugin*p;
	void*data;
	char name[9]; /* canonical name for human usage */
	cl_sem refcount;
};

struct part* cloudvpn_find_part_by_name (char*);

struct part* cloudvpn_part_init (struct plugin*);
int cloudvpn_part_close (struct part*);

#endif
