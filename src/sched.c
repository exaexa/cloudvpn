
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
#include "alloc.h"

static int keep_running;

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

	q = &queue;

	while ( (*q) && ( (*q)->w->priority >= w->priority) ) {
		nw->next = *q;
		*q = nw;
	}

	return 0;
}

int cloudvpn_scheduler_init()
{
	queue = 0;
	keep_running = 1;

	return 0;
}

void cloudvpn_scheduler_destroy()
{
	/* TODO dealloc all that was left in queue */
}

void cloudvpn_scheduler_stop()
{
	keep_running = 0;
}

static void do_work(){
	/* TODO
	 * retrieve work (with mutex) and do it
	 */
}

int cloudvpn_scheduler_run()
{
	while (keep_running)
		do_work();

	return 0;
}

