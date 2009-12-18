
#ifndef _CVPN_CONNECTION_H
#define _CVPN_CONNECTION_H

struct conn_backend_interface_t {
	//loaded function pointers
	/*

	void conn_start_listening(const char*local);
	void conn_stop_listening();

	void conn_connect(const char*remote);
	void conn_disconnect();
	int conn_status();

	int conn_push_data(char*buffer,size_t size);
	int conn_recv_data(char*buffer,size_t size);

	void conn_cb_new_connection();
	void conn_cb_conn_ready();

	*/
};

struct conn_backend_t {
	struct conn_backend_interface_t*handler;
	void *privdata;
};

void conn_backend_load();
void conn_backend_free();

void conn_init(conn_backend_t*);


void conn_cb_poll_add_read(int);
void conn_cb_poll_remove_read(int);
void conn_cb_poll_add_write(int);
void conn_cb_poll_remove_write(int);

#endif

