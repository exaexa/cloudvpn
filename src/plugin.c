
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
#include "alloc.h"

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

static int plugin_safe_remove (struct plugin*p)
{
	/*
	 * remove from linked list
	 */

	struct plugin_list**pl = &plugins;
	struct plugin_list* t;

	if (cl_sem_value (p->refcount) )
		return 1;

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


static struct plugin_list* find_pl_by_plugin (struct plugin*p) {

	struct plugin_list*pl;

	for (pl = plugins;pl;pl = pl->next) if (pl->p == p) return pl;
	return 0;
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

/*
 * library loading stuff
 */

#include <dlfcn.h>

struct plugin* cloudvpn_open_plugin (const char*filename) {

	struct plugin*p;
	void *dl;

	struct plugin* (*plugin_get_func) ();
	int (*plugin_init_func) ();

	dl = dlopen (filename, RTLD_NOW);
	if (!dl) return 0;

	plugin_get_func = dlsym (dl, "cloudvpn_plugin_get");
	if (!plugin_get_func) goto error_getfunc;

	/* initialize the plugin, if possible */
	plugin_init_func = dlsym (dl, "cloudvpn_plugin_init");

	if (plugin_init_func && plugin_init_func() )
		goto error_getfunc;

	p = plugin_get_func();

	if (cl_sem_init (p->refcount, 0) ) goto error_getfunc;

	if (plugin_add (p, dl) ) goto error_plugadd;

	return p;

error_plugadd:
	cl_sem_destroy (p->refcount);

error_getfunc:
	dlclose (dl);

	return 0;
}

int cloudvpn_close_plugin (struct plugin*p)
{
	void * dl;
	struct plugin_list*pl;
	void (*plugin_fini_func) ();

	pl = find_pl_by_plugin (p);
	if (!pl) return 1;
	dl = pl->dlopen_handle;

	/* be sure to do this before unloading, so no one instantiates it */
	if (plugin_safe_remove (p) ) return 2;

	plugin_fini_func = dlsym (dl, "cloudvpn_plugin_fini");
	if (plugin_fini_func) plugin_fini_func();
	dlclose (dl);

	return 0;
}
