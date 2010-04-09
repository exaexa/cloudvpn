
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

#ifndef _CVPN_SHUTDOWN_H
#define _CVPN_SHUTDOWN_H

/*
 * Shutdown is here for graceful exits, and is good for that.
 */

int cloudvpn_shutdown(int); /* if parameter, cloudvpn respawns itself */

#endif

