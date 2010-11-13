
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

/* TODO mutex part list functions */

#include "pool.h"
#include "alloc.h"

/*
 * stuff for remembering active parts, esp. for finding them by name
 */



struct part_list {
	struct part* p;
	struct part_list* next;
};

static struct part_list* parts;
static cl_mutex parts_mutex;

static int part_add (struct part*p)
{
	/*
	 * push the part into linked list
	 */

	struct part_list* pl = cl_malloc (sizeof (struct part_list) );
	if (!pl) return 1;

	cl_mutex_lock (parts_mutex);
	pl->p = p;
	pl->next = parts;

	parts = pl;
	cl_mutex_unlock (parts_mutex);

	return 0;
}

static int part_remove (struct part*p)
{
	/*
	 * remove from linked list
	 */

	struct part_list**pl;
	struct part_list* t;

	cl_mutex_lock (parts_mutex);

	pl = &parts;

	while (*pl) {
		if ( (*pl)->p == p) {
			t = *pl;
			*pl = (*pl)->next;
			cl_free (t);
			cl_mutex_unlock (parts_mutex);
			return 0;
		} else
			pl = & ( (*pl)->next);
	}

	cl_mutex_unlock (parts_mutex);
	return 1;
}

struct part* cloudvpn_find_part_by_name (const char*name) {
	/*
	 * return a pointer to a part found by human name reference.
	 */

	int i;
	struct part_list*pl;
	cl_mutex_lock (parts_mutex);
	for (pl = parts;pl;pl = pl->next) {

		if (!pl->p->name) continue;
		for (i = 0;name[i] && (pl->p->name[i]) &&
		        (name[i] == pl->p->name[i]);
		        ++i);
		if ( (name[i] == 0) && (pl->p->name[i] == 0) ) {
			cl_mutex_unlock (parts_mutex);
			return pl->p;
		}
	}
	cl_mutex_unlock (parts_mutex);
	return 0;
}

/*
 * initialization/deinitialization
 */

struct part* cloudvpn_part_init (struct plugin* plug, const char*name) {
	/*
	 * allocate the structure, increase refcount.
	 *
	 * Bad thing is that we can't let allocation to be done on caller side,
	 * because it would ruin the purpose of having this layer separated.
	 * We also can't count on the caller being available when it's finally
	 * decided to delete the part.
	 */

	int i;

	struct part*p = cl_malloc (sizeof (struct part) );
	if (!p) return 0;

	cl_sem_post (plug->refcount);

	if (part_add (p) ) goto dealloc_error;

	p->p = plug;
	p->data = 0;

	if (cl_sem_init (p->refcount, 1) ) /* got one ref from this right? */
		goto remove_error;

	if (name) { /* copy the name */
		for (i = 0;name[i];++i);
		p->name = cl_malloc (i + 1);
		if (!p->name) goto sem_error;
		p->name[i] = 0;
		for (i = i - 1;i >= 0;--i) p->name[i] = name[i];
	} else p->name = 0;

	/* call the constructor */
	if (p->p->init) p->p->init (p);

	return p;

sem_error:
	cl_sem_destroy (p->refcount);

remove_error:

	part_remove (p);

dealloc_error:

	cl_sem_get (plug->refcount);
	cl_free (p);

	return 0;
}

struct part* cloudvpn_part_acquire (struct part*p) {

	/* only increase refcount */
	cl_sem_post (p->refcount);

	return 0;
}

static void cloudvpn_part_destroy (struct part*p)
{
	part_remove (p);

	/* call the destructor */
	if (p->p->fini) p->p->fini (p);

	cl_sem_get (p->p->refcount);

	cl_sem_destroy (p->refcount);
	cl_free (p);
}

void cloudvpn_part_close (struct part*p)
{
	/* decrease refcount and probably delete the part */
	cl_sem_get (p->refcount);

	if (cl_sem_value (p->refcount) )
		cloudvpn_part_destroy (p);
}

/*
 * init/deinit
 */

int cloudvpn_init_pool()
{
	return cl_mutex_init (&parts_mutex);
}

void cloudvpn_finish_pool()
{
	cl_mutex_destroy (parts_mutex);
}
