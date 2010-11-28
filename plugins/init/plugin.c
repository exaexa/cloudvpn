
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


/*
 * init plugin for cloudvpn.
 *
 * This is intended to be autoloaded on cloudvpn startup, then read some config
 * files and/or receive instructions from something else; then configure
 * cloudvpn parts to act well together.
 *
 * Should come with debug mode, so you can enter debug commands on cmdline.
 * Also may setup PID files etc.
 */

#include "api.h"

/*
 * plugin functions
 */

static void initplugin_process_work (struct part*p, struct work*w)
{
}

static void initplugin_init (struct part*p)
{
}

static void initplugin_fini (struct part*p)
{
}

/*
 * plugin interface
 */

static struct plugin thisplugin;
static const char pl_name[] = "init";

int cloudvpn_plugin_init()
{
	thisplugin.name = pl_name;
	thisplugin.process_work = initplugin_process_work;
	thisplugin.init = initplugin_init;
	thisplugin.fini = initplugin_fini;

	return 0;
}

struct plugin* cloudvpn_plugin_get () {
	return &thisplugin;
}

