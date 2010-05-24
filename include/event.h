
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

#ifndef _CVPN_EVENT_H
#define _CVPN_EVENT_H

/*
 * generic event/poll/timer registration/callbacking interface
 */

#include "timestamp.h"

enum {
	event_fd_readable,
	event_fd_writeable,
	event_time,
	event_signal
};

struct event {
	int type;
	int priority;
	union {
		int fd;
		uint64_t time;
		int signal;
	};
	uint32_t owner;
	void* data;
};

struct event* cloudvpn_new_event ();
void cloudvpn_delete_event (struct event*);

void cloudvpn_register_event (struct event*);
void cloudvpn_unregister_event (struct event*);

void cloudvpn_wait_for_event();

int cloudvpn_event_init();
int cloudvpn_event_finish();

#endif

