
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

int cloudvpn_scheduler_run (int*);

struct work* cloudvpn_new_work();
int cloudvpn_schedule_work (struct work*);

void cloudvpn_schedule_event_poll();

enum {
	work_packet, /* part processes a data packet */
	work_event, /* part is woken up by an event */
	work_poll, /* wait for events */
	work_part_cleanup, /* broadcast about a part being removed */
	work_plugin_cleanup, /* same for plugin */
	work_command /* configuration command/statement (in packet) */
};

#include "packet.h"
#include "pool.h"
#include "event.h"

#define LOWEST_PRIORITY 255

struct work {
	int type;
	uint8_t priority; /* lower number gets processed faster */
	short is_static; /* struct work is owned and freed by someone else */

	union {
		struct packet* p; /* packet to process */
		struct event_data e;
		struct part* pt; /* part to cleanup */
		struct plugin* pl; /* plugin to cleanup */
	};
};

#endif

