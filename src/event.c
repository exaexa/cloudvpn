
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

#include "event.h"
#include "alloc.h"
#include "mutex.h"

#define _XOPEN_SOURCE
#include <ev.h>

struct event* cloudvpn_new_event() {
	return cl_malloc (sizeof (struct event) );
}

void cloudvpn_delete_event (struct event*e)
{
	cl_free (e);
}

static void notify_event_loop()
{

}

void cloudvpn_register_event (struct event*e)
{

	notify_event_loop();
}

void cloudvpn_unregister_event (struct event*e)
{

	notify_event_loop();
}

static cl_mutex event_mutex;

int cloudvpn_event_init()
{
	cl_mutex_init (&event_mutex);
	return 0;
}

int cloudvpn_event_finish()
{
	cl_mutex_destroy (&event_mutex);
	return 0;
}

void cloudvpn_wait_for_event()
{
	/* don't block if there's already other thread waiting */
	if (cl_mutex_trylock (event_mutex) ) return;

	cl_mutex_unlock (event_mutex);
}

