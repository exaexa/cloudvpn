
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

#ifndef _CVPN_MUTEX_H
#define _CVPN_MUTEX_H

/*
 * wrapper around mutual exclusion implementation. If you want to supply any
 * other (more native) please feed it into mutex.c or so. Also including
 * conditions and waiting for them.
 */

typedef void* cl_mutex;
typedef void* cl_cond;

int cl_mutex_init (cl_mutex*);
int cl_mutex_destroy (cl_mutex);
int cl_mutex_lock (cl_mutex);
int cl_mutex_trylock (cl_mutex);
int cl_mutex_unlock (cl_mutex);

int cl_cond_init (cl_cond*);
int cl_cond_destroy (cl_cond);
int cl_cond_wait (cl_cond, cl_mutex);
int cl_cond_signal (cl_cond);
int cl_cond_broadcast (cl_cond);


#endif

