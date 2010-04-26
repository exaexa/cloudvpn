
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

#ifndef _CVPN_TIME_H
#define _CVPN_TIME_H

/*
 * generator of exact timestamps
 */

#include <stdint.h>

/*
 * timestamp = microseconds since epoch. Accurate enough to compute both
 * bandwidths, hires delays, and dates even in long-term processes.
 * NOTE that 64bit integer can measure percise time until 07:36:10 UTC on the
 * 28th of May year 60425 after Christ's birth. Then it might turn into a fixme.
 */

#ifndef __TIMESTAMP_C__
extern
#endif
	uint64_t timestamp_lasttime;

inline uint64_t cloudvpn_timestamp();

void cloudvpn_timestamp_update();


#endif

