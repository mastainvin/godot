//
// Created by vincent on 15/02/24.
//

#include "xumes_communication.h"

#include <iostream>

#include "core/variant/dictionary.h"
#include "core/os/os.h"


void XumesCommunication::thread_func(void *p_udata) {
	XumesCommunication *ac = (XumesCommunication *) p_udata;
	uint64_t msdelay = 1000;

	ac->init_socket();

	uint16_t port;
	while(!ac->exit_thread) {
		print_line("Waiting for connection...");
		ac->wait_connection();
		print_line("Connected!");
		print_line("Waiting for command...");
		Dictionary data = ac->wait_call();

		print_line("Command received!");
		Dictionary res;
		if (data.has("command")) {
			String command = data["command"];
			if (command == "get_features") {

			} else if (command == "start_scenario") {
				if (data.has("name")) {
					print_line("Instance creation...");
					port = ScenarioRunner::get_instance()->get_port();
					ScenarioRunner::get_instance()->run(data["name"]);
					res["port"] = port;
					print_line("Instance created!");
				}
			}
		}
		OS::get_singleton()->delay_usec(msdelay * 1000);

		print_line("Pushing PORT!");
		ac->push_info(res);

		ac->disconnection();
		print_line("Disconnected!");
	}
}

bool XumesCommunication::init_socket() {

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		return false;
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		return false;
	}

	return true;

}

bool XumesCommunication::wait_connection() {
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
		perror("accept");
		return false;
	}


	return true;
}

void XumesCommunication::disconnection() {
	close(new_socket);
}

void XumesCommunication::stop_socket() {
	print_line("stop_socket");
	close(server_fd);
	close(new_socket);
}


Dictionary XumesCommunication::wait_call() {

	valread = read(new_socket, buffer, 1024);

	if (valread < 0) {
		return Dictionary();
	}

	String str = String(buffer);
	json.parse(str);

	Dictionary data = Dictionary(json.get_data());

	memset(buffer, 0, sizeof(buffer));

	return data;
}

bool XumesCommunication::push_info(Dictionary &data) {
	String str = json.stringify(data, "", true, true);
	strcpy(buffer, str.utf8());
	ssize_t r = send(new_socket, buffer, strlen(buffer), 0);
	if (r == -1) {
		return false;
	}

	memset(buffer, 0, sizeof(buffer));

	return true;
}



Error XumesCommunication::init() {

	if (!started) {
		thread_exited = false;

		mutex = new Mutex();
		thread = new Thread();


		thread->start(XumesCommunication::thread_func, this);

		started = true;
	}



	return OK;
}

XumesCommunication *XumesCommunication::singleton = NULL;

XumesCommunication *XumesCommunication::get_singleton() {
	return singleton;
}


void XumesCommunication::unlock() {
	if (!thread || !mutex) {
		return;
	}

	mutex->unlock();
}

void XumesCommunication::lock() {
	if (!thread || !mutex) {
		return;
	}

	mutex->lock();
}

void XumesCommunication::finish() {
	if (!thread) {
		return;
	}

	exit_thread = true;
	//thread->wait_to_finish();

	memdelete(thread);

	if (mutex) {
		memdelete(mutex);
	}

	thread = NULL;
}

void XumesCommunication::_bind_methods() {}

XumesCommunication::XumesCommunication() {
	singleton = this;
}

