
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
 * wrapper around mutual exclusion stuff. If you want to supply any other (more
 * native) implementations, please feed them to mutex.c or so.
 */

typedef void* cl_mutex;
typedef void* cl_cond;
typedef void* cl_sem;

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

int cl_sem_init (cl_sem*, unsigned int value);
int cl_sem_destroy (cl_sem);
int cl_sem_post (cl_sem);
int cl_sem_get (cl_sem);
int cl_sem_value (cl_sem);


#endif

