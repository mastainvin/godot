//
// Created by vincent on 15/02/24.
//

#ifndef GODOT_XUMES_COMMUNICATION_H
#define GODOT_XUMES_COMMUNICATION_H


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

#include "scene/main/node.h"

#include "core/object/object.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "core/string/print_string.h"
#include "core/io/json.h"

#include "scenario_runner.h"

class XumesCommunication : public Object {
	GDCLASS(XumesCommunication, Object);

	static XumesCommunication *singleton;
	static void thread_func(void *p_udata);

private:
	bool thread_exited;
	mutable bool exit_thread;

	Thread *thread;
	Mutex *mutex;

	bool started = false;

	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	JSON json = JSON();

	bool wait_connection();
	void disconnection();
	void stop_socket();
	bool init_socket();
	Dictionary wait_call();
	bool push_info(Dictionary &data);


public:
	static XumesCommunication *get_singleton();

	Error init();
	void lock();
	void unlock();
	void finish();

protected:
	static void _bind_methods();

public:

	XumesCommunication();
};

#endif //GODOT_XUMES_COMMUNICATION_H
