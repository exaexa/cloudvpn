
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

#ifndef _CVPN_PLUGIN_H
#define _CVPN_PLUGIN_H

/*
 * plugin is a dynamically loaded library that every part is made of.
 * This can be router, packet filter, packet queueing framework, traffic
 * endpoints, tunnels or interfaces, etc.
 */

#include "packet.h"
#include "pool.h"
#include "sched.h"
#include "mutex.h"

struct plugin {
	const char*name;
	cl_sem refcount; /* number of instances of the plugin */

	void (*process_work) (struct part*, struct work*);
	void (*init) (struct part*);
	void (*fini) (struct part*);
};

struct plugin* cloudvpn_find_plugin_by_name (const char*);
void cloudvpn_open_plugin (const char*);
void cloudvpn_close_plugin (struct plugin*);

#endif

