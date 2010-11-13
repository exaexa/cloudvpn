
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

#include "core.h"

#include "event.h"
#include "sched.h"

int cloudvpn_core_init()
{
	if (cloudvpn_event_init() ) return 1;
	if (cloudvpn_scheduler_init() ) return 2;
	if (cloudvpn_init_plugins() ) return 3;
	if (cloudvpn_init_pool() ) return 4;
	return 0;
}

int cloudvpn_core_finish()
{
	cloudvpn_finish_pool();
	cloudvpn_finish_plugins();
	if (cloudvpn_scheduler_destroy() ) return 2;
	if (cloudvpn_event_finish() ) return 1;
	return 0;
}

