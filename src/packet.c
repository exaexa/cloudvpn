
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

#include "packet.h"
#include "alloc.h"

struct packet* cloudvpn_packet_alloc() {
	return cl_calloc (1, sizeof (struct packet) ); /* note the zeroes! */
}

void cloudvpn_packet_free (struct packet* p)
{
	if (p->data) cl_free (p->data);
	cl_free (p);
}

int cloudvpn_alloc_data (struct packet* p)
{
	char*t = cl_realloc (p->data, p->len);

	if (t) {
		p->data = t;
		return 0;
	} else
		return 1;
}

