
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

#ifndef _CVPN_API_H
#define _CVPN_API_H

/*
 * API is meant as a bridge between core part and plugins. Plugin includes this
 * file exactly once with defined __CVPN_PLUGIN_MAIN, which gives it a nice
 * global namespace of all functions that it could need. There are also
 * declarations of functions that must be exported by the script, and those
 * that are exported to the script, when included with __CVPN_PLUGIN defined.
 * Otherwise it justs declares the API functions.
 */

#include "plugin.h"

#define CLOUDVPN_API_VERSION 0x1001

#if defined(__CVPN_PLUGIN_MAIN)

/*
 * place to store functions exported to the plugin
 */

int (*cloudvpn_get_version)() = 0;
/* ... more to come */

/*
 * loading function. Performs a simple API version check.
 */

int cloudvpn_plugin_load(int version, void**functions)
{
	if(functions[0] != (void*)CLOUDVPN_API_VERSION) return 1;
	cloudvpn_get_version = functions[0];
	return 0;
}

#elif defined(__CVPN_PLUGIN)

/*
 * headers of functions exported to the plugin
 */

extern int (*cloudvpn_get_version)(); 
/* ... more to come */

/*
 * functions that must be exported by the plugin
 */

/*
 * get_plugin_info returns a pointer to statically available plugin structure
 * describing n-th contained plugin, or zero on error.
 */
struct plugin* cloudvpn_plugin_get(int);

#else

/*
 * headers of functions that are part of the API
 */

int cloudvpn_get_version();
/* ... more to come */

#endif


#endif
