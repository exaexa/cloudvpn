
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

static int keep_running;

void cloudvpn_scheduler_exit() {
	keep_running=0;
}

int cloudvpn_scheduler_run() {
	keep_running=1;
	while(keep_running);
	return 0;
}

