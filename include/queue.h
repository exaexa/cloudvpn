
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

#ifndef _CVPN_QUEUE_H
#define _CVPN_QUEUE_H

/*
 * queue is here as an intermediate place for packets that travel among parts.
 */

int cloudvpn_queue_push(struct packet*);
struct packet* cloudvpn_queue_pop();
int cloudvpn_queue_size();

#endif

