
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

/* TODO mutex plugin list operation */

#include "plugin.h"

struct plugin_list {
	struct plugin* p;
	void* dlopen_handle;

	struct plugin_list*next;
};

static struct plugin_list* plugins;

static int plugin_add (struct plugin*p, void*dl_handle)
{
	/*
	 * push the plugin into linked list
	 */

	struct plugin_list* pl = cl_malloc (sizeof (struct plugin_list) );
	if (!pl) return 1;

	pl->p = p;
	pl->next = plugins;
	pl->dlopen_handle = dl_handle;

	plugins = pl;

	return 0;
}

static int plugin_remove (struct plugin*p)
{
	/*
	 * remove from linked list
	 */

	struct plugin_list**pl = &plugins;
	struct plugin_list* t;

	while (*pl) {
		if ( (*pl)->p == p) {
			t = *pl;
			*pl = (*pl)->next;
			cl_free (t);
			return 0;
		} else
			pl = & ( (*pl)->next);
	}

	return 1;
}


static struct plugin_list* find_pl_by_name (const char* name) {
	int i;
	struct plugin_list*pl;
	for (pl = plugins;pl;pl = pl->next) {

		if (!pl->p->name) continue;
		for (i = 0;name[i] && (pl->p->name[i]) &&
		        (name[i] == pl->p->name[i]);
		        ++i);
		if ( (name[i] == 0) && (pl->p->name[i] == 0) ) return pl;
	}
	return 0;
}

struct plugin* cloudvpn_find_plugin_by_name (const char* name) {
	struct plugin_list*pl = find_pl_by_name (name);
	if (pl) return pl->p;
	else return 0;
}

struct plugin* cloudvpn_open_plugin (const char*filename) {
	return 0;
}

int cloudvpn_close_plugin (struct plugin*p)
{
	if (cl_sem_value (p->refcount) ) {
		return 1;
	}

	/*TODO*/
	return 0;
}
