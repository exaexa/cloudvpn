
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

#ifndef _CVPN_ALLOC_H
#define _CVPN_ALLOC_H

/*
 * Simple wrapper around the memory allocating functions. If you want to
 * replace them, do it here.
 */

#include <stdlib.h>

#define cl_malloc malloc
#define cl_calloc calloc
#define cl_free free
#define cl_realloc realloc

#endif

