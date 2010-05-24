
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

static cl_mutex event_mutex;
static struct ev_loop* loop;
static ev_async async;

struct event* cloudvpn_new_event() {
	return cl_malloc (sizeof (struct event) );
}

void cloudvpn_delete_event (struct event*e)
{
	cl_free (e);
}

static void async_callback (EV_P_ ev_async*w, int revents) {}

static void notify_event_loop()
{
	/* asynchronously interrupt sleep so libev can update itself */
	ev_async_init (&async, async_callback);
	ev_async_send (loop, &async);
}

void cloudvpn_register_event (struct event*e)
{
	notify_event_loop();
}

void cloudvpn_unregister_event (struct event*e)
{
	notify_event_loop();
}

int cloudvpn_event_init()
{
	loop = ev_default_loop (0);

	return loop || cl_mutex_init (&event_mutex);
}

int cloudvpn_event_finish()
{
	return cl_mutex_destroy (event_mutex);
}

void cloudvpn_wait_for_event()
{
	/* don't block if there's already other thread waiting */
	if (cl_mutex_trylock (event_mutex) ) return;

	cl_mutex_unlock (event_mutex);
}

