
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

#include "sched.h"
#include "event.h"
#include "alloc.h"
#include "mutex.h"

/*
 * simple list that contains tasks that need to be done.
 *
 * I can think of modifying it to something better
 */

struct work_queue {
	struct work_queue*next;
	struct work*w;
};

static struct work_queue *queue;
static cl_mutex queue_mutex;

struct work* cloudvpn_new_work() {
	return cl_malloc (sizeof (struct work) ); /* no-brainer */
}

int cloudvpn_schedule_work (struct work*w) /* insert work into the queue */
{
	struct work_queue** q;
	struct work_queue* nw;

	nw = cl_malloc (sizeof (struct work_queue) );
	if (!nw) return 1;
	nw->w = w;

	cl_mutex_lock(queue_mutex);

	q = &queue;

	while ( (*q) && ( (*q)->w->priority >= w->priority) ) {
		nw->next = *q;
		*q = nw;
	}

	cl_mutex_unlock(queue_mutex);

	return 0;
}

int cloudvpn_scheduler_init()
{
	queue = 0;
	return cl_mutex_init(&queue_mutex);
}

void cloudvpn_scheduler_destroy()
{
	struct work_queue*p;

	while(queue) {
		p=queue;
		queue=queue->next;
		cl_free(p->w);
		cl_free(p);
	}

	cl_mutex_destroy(queue_mutex);
}

static void do_work(struct work* w)
{
	switch(w->type) {
	case work_packet:
		break;
	case work_event:
		break;
	case work_poll:
		break;
	case work_exit:
		break;
	}
}

int cloudvpn_scheduler_run(int* keep_running)
{
	struct work_queue*p;
	struct work*w;
	while (*keep_running) {

		cl_mutex_lock(queue_mutex);

		if(!queue) {

			cl_mutex_unlock(queue_mutex);
			cloudvpn_wait_for_event();

		} else {

			p=queue;
			queue=queue->next;

			cl_mutex_unlock(queue_mutex);

			w=p->w;
			cl_free(p);
			do_work(w);
			cl_free(w);
		}
	}

	return 0;
}

