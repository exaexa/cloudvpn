
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
#include "sched.h"

#define _XOPEN_SOURCE
#include <ev.h>

static void reload_event_loop();

/*
 * because we need something internal in event struct, we will create it with
 * a little tail that is not in declaration in header file.
 *
 * This adds abstraction (and plugins don't need to include ev.h too).
 *
 * Well, and it's an ugly trick.
 */

#define internal(e) ((struct event_internal_data*)(e+1))

struct event_internal_data {
	union {
		ev_io w_io;
		ev_signal w_signal;
		ev_timer w_timer;
		/* async events are handled internally by cloudvpn */
	};
};

/*
 * frontend just pushes events to the list that is processed by event core
 * whenever it has time for it.
 */

struct event* cloudvpn_new_event() {
	return cl_malloc (sizeof (struct event)
	                  + sizeof (struct event_internal_data) );
}

void cloudvpn_delete_event (struct event*e)
{
	cl_free (e);
}

typedef enum {add, remove, send_async} eventlist_op;

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

int cloudvpn_event_send_async (struct event*e)
{
	return push_event_change (send_async, e);
}

/*
 * event core functions
 */

static cl_mutex eventcore_mutex;
static struct ev_loop* loop;
static ev_async async;

static void null_async_callback (EV_P_ ev_async*w, int revents) {}

static void reload_event_loop()
{
	/* asynchronously interrupt sleep so libev can update itself */
	ev_async_send (loop, &async);
}

int cloudvpn_event_init()
{
	loop = ev_default_loop (0);

	ev_async_init (&async, null_async_callback);
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
 * several callbacks for various types of libev watchers
 *
 * I would totally do a template lol.
 */

static int schedule_event (struct event*e);

static void libev_io_cb (struct ev_loop *loop, ev_io *w, int revents)
{
	struct event*e;
	e = w->data;

	schedule_event (e);
}

static void libev_timer_cb (struct ev_loop *loop, ev_timer *w, int revents)
{
	struct event*e;
	e = w->data;

	schedule_event (e);
}

static void libev_signal_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
	struct event*e;
	e = w->data;

	schedule_event (e);
}

/*
 * two functions that add/remove events to real libev loop. Called safely when
 * loop is guaranteed not to be running
 */

static void add_handler (struct event*e)
{
	struct event_internal_data*i;
	i = internal (e);

	switch (e->data.type) {

	case event_time:
		ev_timer_init (& (i->w_timer), libev_timer_cb,
		               0.000001f*e->data.time, 0);
		ev_timer_start (loop, & (i->w_timer) );
		break;

	case event_signal:
		ev_signal_init (& (i->w_signal), libev_signal_cb,
		                e->data.signal);
		ev_signal_start (loop, & (i->w_signal) );
		break;

	case event_fd_writeable:
		ev_io_init (& (i->w_io), libev_io_cb, e->data.fd, EV_WRITE);
		ev_io_start (loop, & (i->w_io) );
		break;

	case event_fd_readable:
		ev_io_init (& (i->w_io), libev_io_cb, e->data.fd, EV_READ);
		ev_io_start (loop, & (i->w_io) );
		break;
	}
}

static void remove_handler (struct event*e)
{
	struct event_internal_data*i;
	i = internal (e);

	switch (e->data.type) {
	case event_time:
		ev_timer_stop (loop, & (i->w_timer) );
		break;

	case event_signal:
		ev_signal_stop (loop, & (i->w_signal) );
		break;

	case event_fd_writeable:
	case event_fd_readable:
		ev_io_stop (loop, & (i->w_io) );
		break;
	}
}

/*
 * event processing stuff
 */

static void cleanup_event (struct event*e)
{
	/*
	 * always remove the handler, so it doesn't trigger again in next
	 * thread (faster than event level-trigger is handled)
	 */

	remove_handler (e);

	if (!e->is_static) {

		cloudvpn_delete_event (e);
	}
}

static int schedule_event (struct event*e)
{
	struct work*w;

	w = cloudvpn_new_work();

	if (!w) return 1;

	w->type = work_event;
	w->priority = e->priority;
	w->is_static = 0;

	memcpy (&w->e, &e->data, sizeof (struct event_data) );

	cleanup_event (e);

	return cloudvpn_schedule_work (w);
}

/*
 * event waiting frontend
 */

void cloudvpn_wait_for_event()
{
	int created_async_work;

	/* don't block if there's already other thread waiting */
	if (cl_mutex_trylock (eventcore_mutex) ) return;

	/* load stuff from frontend, put it to ev, wait for it. */

	cl_mutex_lock (ecq_mutex);

	created_async_work = 0;

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
		case send_async:
			schedule_event (q->e);
			++created_async_work;
			break;
		}
		q = q->next;
	}

#	undef q

	cl_mutex_unlock (ecq_mutex);

	/* don't wait if it seems that we have other work to do. */
	if (!created_async_work)
		ev_loop (loop, EVLOOP_ONESHOT);

	cl_mutex_unlock (eventcore_mutex);
}

