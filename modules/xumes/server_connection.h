//
// Created by vincent on 13/02/24.
//

#ifndef GODOT_SERVER_CONNECTION_H
#define GODOT_SERVER_CONNECTION_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <map>

#include "core/string/print_string.h"
#include "core/io/json.h"

class ServerConnection {
	static std::map<__pid_t, ServerConnection*> serverConnections;

private:
	bool connected = false;

	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	JSON json = JSON();


public:


	bool init_socket(uint16_t port);
	void stop_socket();

	bool wait_connection();
	Dictionary get(bool &r);
	bool post_dict(const Dictionary &data);
	bool post_int(int response);
	bool post_variant(const Variant &variant);

	bool is_connected() {return connected;}

	static void save(ServerConnection *newServer) {
		serverConnections[getpid()] = newServer;
	}

	static ServerConnection* load() {
		return serverConnections[getpid()];
	}

	static bool is_saved() {
		return serverConnections.count(getpid()) > 0;
	}

};

#endif //GODOT_SERVER_CONNECTION_H
