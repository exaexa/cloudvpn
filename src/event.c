
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
	eventlist_op op;
};

static struct eventlist* event_change_queue;
static cl_mutex ecq_mutex;

static int push_event_change (eventlist_op op, struct event*e)
{
	/* insert event to registration queue */
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

/*
 * this gets called after event result is scheduled.
 */

static void cleanup_event (struct event*e)
{
	if (!e->is_static) {

		/* we can delete the event manually, because we are callbacked
		 * by code that is mutexed correctly, also dodging OOM
		 * problems. */

		remove_handler (e);
		cloudvpn_delete_event (e);
	}
}

/*
 * several callbacks for various types of libev watchers
 *
 * I would totally do a template lol.
 */

static void libev_io_cb (struct ev_loop *loop, ev_io *w, int revents)
{
	struct event*e;
	e = w->data;

	cleanup_event (e);
}

static void libev_timer_cb (struct ev_loop *loop, ev_timer *w, int revents)
{
	struct event*e;
	e = w->data;

	cleanup_event (e);
}

static void libev_signal_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
	struct event*e;
	e = w->data;

	cleanup_event (e);
}

static void libev_async_cb (struct ev_loop *loop, ev_async *w, int revents)
{
	struct event*e;
	e = w->data;

	cleanup_event (e);
}

/*
 * two functions that add/remove events to real libev loop
 */

static int add_handler (struct event*e)
{

	return 0;
}

static int remove_handler (struct event*e)
{

	return 0;
}

/*
 * event waiting frontend
 */

void cloudvpn_wait_for_event()
{
	/* don't block if there's already other thread waiting */
	if (cl_mutex_trylock (eventcore_mutex) ) return;

	/* load stuff from frontend, put it to ev, wait for it. */

	cl_mutex_lock (ecq_mutex);

	/* i'm so lazy */
#	define q event_change_queue

	while (q) {
		switch (q->op) {
		case add:
			add_handler (q->e);
			break;
		case remove:
			remove_handler (q->e);
			break;
		}
		q = q->next;
	}

#	undef q

	cl_mutex_unlock (ecq_mutex);

	ev_loop (loop, EVLOOP_ONESHOT);

	cl_mutex_unlock (eventcore_mutex);
}

