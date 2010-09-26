
#include "pool.h"

struct part* cloudvpn_find_part_by_name (const char*) {
	/*
	 * return a pointer to a part found by human name reference.
	 */

	return 0;
}

struct part* cloudvpn_part_init (struct plugin*) {
	/*
	 * allocate the structure, increase refcount.
	 *
	 * Bad thing is that we can't let allocation to be done on caller side,
	 * because it would ruin the purpose of having this layer separated.
	 * We also can't count on the caller being available when it's finally
	 * decided to delete the part.
	 */

	return 0;
}

struct part* cloudvpn_part_acquire (struct part*) {
	/* only increase refcount */

	return 0;
}

void cloudvpn_part_close (struct part*)
{
	/* decrease refcount and probably delete the part */
}
