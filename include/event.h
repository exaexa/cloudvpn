
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

enum cloudvpn_event_type {
	ev_fd_read,
	ev_fd_write,
	ev_fd_error,
	ev_timer,
	ev_periodic,
	ev_signal,
	ev_immediate_work
};

typedef void (*cloudvpn_event_cb) (enum cloudvpn_event_type, int, void*);

int cloudvpn_register_event (enum cloudvpn_event_type, int, cloudvpn_event_cb);
int cloudvpn_unregister_event (enum cloudvpn_event_type, int);

void cloudvpn_wait_for_event();

#endif

