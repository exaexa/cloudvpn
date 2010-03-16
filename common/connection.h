
#ifndef _CVPN_CONNECTION_H
#define _CVPN_CONNECTION_H

struct conn_backend;

struct conn_backend_interface {

	/*
	 * internals, use those sporadically
	 */

	int (*init) (struct conn_backend*);
	int (*finish) (struct conn_backend*);

	int (*connect) (struct conn_backend*, const char*);
	int (*listen) (struct conn_backend*, const char*);

	/*
	 * interface to use
	 */

	int (*accept) (struct conn_backend*, struct conn_backend**);
	int (*close) (struct conn_backend*);

	int (*get_status) (struct conn_backend*);
	int (*get_peer_description) (struct conn_backend*, char*, size_t);

	int (*can_send_size) (struct conn_backend*, size_t);
	int (*send_packet) (struct conn_backend*, const char*, size_t, int);
	int (*recv_packet) (struct conn_backend*, const char*, size_t);

	int (*periodic_update) (struct conn_backend*);
};

struct conn_backend {
	struct conn_backend_interface*handler;
	void *privdata;

	void *callback_arg;
	void (*callback_func) (int, void*);
};

/*
 * global initialization/denitialization
 */

void conn_init();
void conn_finish();

/*
 * those 2 functions are used to initially create a communication backend
 *
 * (accept() alternative is elsewhere)
 */

conn_backend* conn_connect (const char*);
conn_backend* conn_listen (const char*);
void conn_free (conn_backend*);

/*
 * those functions are exported to the plugin's space, so it can
 * process polling events
 */

void conn_cb_poll_add_read (int);
void conn_cb_poll_remove_read (int);
void conn_cb_poll_add_write (int);
void conn_cb_poll_remove_write (int);

/*
 * interfacing macros/functions
 *
 * TODO
 */

#endif

