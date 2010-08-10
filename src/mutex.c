

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

#include "mutex.h"

/*
 * this wraps pthread mutexes. Feel free to supply anything better.
 */

#include "alloc.h"
#include <pthread.h>
#include <semaphore.h>

int cl_mutex_init (cl_mutex* mp)
{
	*mp = cl_malloc (sizeof (pthread_mutex_t) );
	if (! (*mp) ) return 1;
	if (!pthread_mutex_init ( (pthread_mutex_t*) *mp, 0) ) return 0;
	cl_free (*mp);
	return 1;
}

int cl_mutex_destroy (cl_mutex m)
{
	int r = pthread_mutex_destroy ( (pthread_mutex_t*) m);
	cl_free (m);
	return r;
}

int cl_mutex_lock (cl_mutex m)
{
	return pthread_mutex_lock ( (pthread_mutex_t*) m);
}

int cl_mutex_trylock (cl_mutex m)
{
	return pthread_mutex_trylock ( (pthread_mutex_t*) m);
}

int cl_mutex_unlock (cl_mutex m)
{
	return pthread_mutex_unlock ( (pthread_mutex_t*) m);
}


int cl_cond_init (cl_cond* cp)
{
	*cp = cl_malloc (sizeof (pthread_cond_t) );
	if (! (*cp) ) return 1;
	if (!pthread_cond_init ( (pthread_cond_t*) *cp, 0) ) return 0;
	cl_free (*cp);
	return 0;
}

int cl_cond_destroy (cl_cond c)
{
	int r = pthread_cond_destroy ( (pthread_cond_t*) c);
	cl_free (c);
	return r;
}

int cl_cond_wait (cl_cond c, cl_mutex m)
{
	return pthread_cond_wait ( (pthread_cond_t*) c, (pthread_mutex_t*) m);
}

int cl_cond_signal (cl_cond c)
{
	return pthread_cond_signal ( (pthread_cond_t*) c);
}

int cl_cond_broadcast (cl_cond c)
{
	return pthread_cond_broadcast ( (pthread_cond_t*) c);
}

int cl_sem_init (cl_sem* sp, unsigned int value)
{
	*sp = cl_malloc (sizeof (sem_t) );
	if (! (*sp) ) return 1;
	if (!sem_init ( (sem_t*) sp, 0, value) ) return 0;
	cl_free (*sp);
	return 0;
}

int cl_sem_destroy (cl_sem s)
{
	int r = sem_destroy ( (sem_t*) s);
	cl_free (s);
	return r;
}

int cl_sem_post (cl_sem s)
{
	return sem_post ( (sem_t*) s);
}

int cl_sem_get (cl_sem s)
{
	return sem_trywait ( (sem_t*) s);
}

int cl_sem_value (cl_sem s)
{
	int v;
	if (! (sem_getvalue ( (sem_t*) s, &v) ) ) return v;
	else return -1;
}

