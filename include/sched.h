
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

#ifndef _CVPN_SCHED_H
#define _CVPN_SCHED_H

/*
 * scheduler is an intermediate place for tasks that travel among parts.
 * It can also handle multicore tasks, etc.
 */

int cloudvpn_scheduler_init();
int cloudvpn_scheduler_destroy();

int cloudvpn_scheduler_run(int*);

/* work priority
 *
 * 0 = critical, above everything (like for signals or whatever. probably not used)
 * 1 = common stuff
 * 2 = idle (polling etc.)
 */

struct work* cloudvpn_new_work();
int cloudvpn_schedule_work (struct work*);

enum {
	work_packet, /* part processes a packet */
	work_event, /* part is woken up by an event */
	work_poll, /* wait for events */
	work_exit /* part is removed */
};

#include "packet.h"
#include "pool.h"

struct work {
	int type;
	int priority;
	union {
		struct packet* p;
		struct {
			uint32_t owner;
			void* event_data;
			int fd;
		};
	};
};

#endif

