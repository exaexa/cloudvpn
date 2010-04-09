
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

#ifndef _CVPN_BOOT_H
#define _CVPN_BOOT_H

/*
 * to boot, we read the config and then startup everything.
 *
 * If anyone wants to reconfigure stuff later, he also wants to load proper
 * plugin to do so.
 */

int boot (int argc, char**argv);

#endif

