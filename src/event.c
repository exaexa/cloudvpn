
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

static void reload_event_loop();

/*
 * frontend just pushes events to the list that is processed by event core
 * whenever it has time for it.
 */

struct event* cloudvpn_new_event() {
	return cl_malloc (sizeof (struct event) );
}

void cloudvpn_delete_event (struct event*e)
{
	cl_free (e);
}

typedef enum {add, remove} eventlist_op;

struct eventlist {
	struct event*e;
	struct eventlist*next;
	eventlist_op op; /* otherwise remove */
};

static struct eventlist* event_change_queue;
static cl_mutex ecq_mutex;

static int push_event_change (eventlist_op op, struct event*e)
{
	struct eventlist*ne;

	ne = cl_malloc (sizeof (struct eventlist) );

	if (!ne) return 1;

	ne->e = e;
	ne->op = op;

	cl_mutex_lock (ecq_mutex);

	ne->next = event_change_queue;
	event_change_queue = ne;

	cl_mutex_unlock (ecq_mutex);

	reload_event_loop();

	return 0;
}

int cloudvpn_register_event (struct event*e)
{
	return push_event_change (add, e);
}

int cloudvpn_unregister_event (struct event*e)
{
	return push_event_change (remove, e);
}


/*
 * event core functions
 */

static cl_mutex eventcore_mutex;
static struct ev_loop* loop;
static ev_async async;

static void async_callback (EV_P_ ev_async*w, int revents) {}

static void reload_event_loop()
{
	/* asynchronously interrupt sleep so libev can update itself */
	ev_async_send (loop, &async);
}

int cloudvpn_event_init()
{
	loop = ev_default_loop (0);

	ev_async_init (&async, async_callback);
	ev_async_start (loop, &async);

	return loop
	       || cl_mutex_init (&eventcore_mutex)
	       || cl_mutex_init (&ecq_mutex);
}

int cloudvpn_event_finish()
{
	return cl_mutex_destroy (eventcore_mutex)
	       || cl_mutex_destroy (ecq_mutex);
}

void cloudvpn_wait_for_event()
{
	/* don't block if there's already other thread waiting */
	if (cl_mutex_trylock (eventcore_mutex) ) return;

	/* load stuff from frontend (tear off and convert list to queue) */

	/*TODO*/

	ev_loop (loop, EVLOOP_ONESHOT);

	cl_mutex_unlock (eventcore_mutex);
}

