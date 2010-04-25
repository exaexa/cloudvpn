
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

#include "boot.h"
#include "core.h"
#include "event.h"

#include <stdio.h>

static int exit = 0;

void cloudvpn_exit()
{
	exit = 1;
}

int main (int argc, char**argv)
{
	if (cloudvpn_core_init() ) return 1;
	if (cloudvpn_boot (argc, argv) ) return 2;
	while (!exit) cloudvpn_wait_for_event();
	if (!cloudvpn_core_finish() ) return 3;

	return 0;
}

