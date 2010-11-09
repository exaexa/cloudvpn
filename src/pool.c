
#include "pool.h"
#include "alloc.h"

struct part* cloudvpn_find_part_by_name (const char*name) {
	/*
	 * return a pointer to a part found by human name reference.
	 */

	return 0;
}

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

	p->p = plug;
	cl_sem_post(plug->refcount);

	p->data = 0;

	if (name) { /* copy the name */
		for (i = 0;name[i] && (i < 8);++i) p->name[i] = name[i];
		p->name[i] = 0;
	}

	cl_sem_init (p->refcount, 1); /* got one ref from this right? */

	/* call the constructor */
	if (p->p->init) p->p->init (p);

	return p;
}

struct part* cloudvpn_part_acquire (struct part*p) {

	/* only increase refcount */
	cl_sem_post (p->refcount);

	return 0;
}

static void cloudvpn_part_destroy (struct part*p)
{
	/* call the destructor */
	if (p->p->fini) p->p->fini (p);

	cl_sem_get(p->p->refcount);

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
