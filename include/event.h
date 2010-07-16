
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

#include <stdint.h>

enum {
	event_fd_readable,
	event_fd_writeable,
	event_time, /* used to wait for some time */
	event_async, /* synchronization of asynchronous events */
	event_signal /* system signal received */
};

struct event_data {
	int type;
	union {
		int fd;
		uint64_t time;
		int signal;
	};
	uint32_t owner;
	void* priv;
};

struct event {
	uint8_t priority;
	short is_static;
	struct event_data data;
};

/* please note that struct event DOESN'T work if not allocated by this */
struct event* cloudvpn_new_event ();
void cloudvpn_delete_event (struct event*);

int cloudvpn_register_event (struct event*);
int cloudvpn_unregister_event (struct event*);
int cloudvpn_event_send_async (struct event*);

void cloudvpn_wait_for_event();

int cloudvpn_event_init();
int cloudvpn_event_finish();

#endif

