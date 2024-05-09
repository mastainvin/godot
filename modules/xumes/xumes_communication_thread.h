//
// Created by vincent on 15/02/24.
//

#ifndef GODOT_XUMES_COMMUNICATION_THREAD_H
#define GODOT_XUMES_COMMUNICATION_THREAD_H


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>


#include "scene/main/node.h"

#include "core/object/object.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "core/string/print_string.h"
#include "core/io/json.h"

#include "xumes_communication.h"
#include <iostream>



class XumesCommunicationThread final : public  Object {
	GDCLASS(XumesCommunicationThread, Object);

	static XumesCommunicationThread *singleton;
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


public:


	static XumesCommunicationThread *get_singleton();

	Error init();
	void lock();
	void unlock();
	void finish();

protected:
	static void _bind_methods();

public:

	XumesCommunicationThread();
};

#endif //GODOT_XUMES_COMMUNICATION_THREAD_H
