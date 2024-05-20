//
// Created by vincent on 13/02/24.
//

#include <climits>
#include <cstring>
#include <map>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server_connection.h"

std::map<__pid_t, ServerConnection*> ServerConnection::serverConnections;

bool ServerConnection::init_socket(uint16_t port) {

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	struct timeval timeout;
	timeout.tv_sec = 10;  // Set the timeout to 10 seconds
	timeout.tv_usec = 0;

	if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port); // let the OS choose

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		return false;
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		return false;
	}

	return true;
}

bool ServerConnection::wait_connection() {
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
		perror("accept");
		return false;
	}

	struct timeval timeout;
	timeout.tv_sec = 10;  // Set the timeout to 10 seconds
	timeout.tv_usec = 0;

	if (setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(new_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	connected = true;
	return true;
}

void ServerConnection::stop_socket() {
	print_line("stopping socket");
	close(server_fd);
	close(new_socket);
	connected = false;
}

Dictionary ServerConnection::get(bool &r) {
	valread = recvfrom(new_socket, buffer, 1024, 0, (struct sockaddr *)&address, (socklen_t*)&addrlen);

	if (valread < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			perror("recvfrom timeout");
		} else {
			perror("recvfrom error");
		}
		r = false;
		exit(EXIT_FAILURE); // Exit on timeout or error
	}

	r = true;
	String str = String(buffer);
	json.parse(str);

	Dictionary data = Dictionary(json.get_data());

	memset(buffer, 0, sizeof(buffer));
	return data;
}

bool ServerConnection::post_dict(const Dictionary &data) {
	String str = json.stringify(data, "", true, true);

	strcpy(buffer, str.utf8().get_data());
	ssize_t r = send(new_socket, buffer, strlen(buffer), 0);

	if (r == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			perror("send timeout");
		} else {
			perror("send error");
		}
		memset(buffer, 0, sizeof(buffer));
		exit(EXIT_FAILURE); // Exit on timeout or error
	}

	memset(buffer, 0, sizeof(buffer));
	return true;
}

bool ServerConnection::post_int(int response) {
	char str[2];
	sprintf(str, "%d", response);
	strcpy(buffer, str);
	ssize_t r = send(new_socket, buffer, strlen(buffer), 0);

	if (r == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			perror("send timeout");
		} else {
			perror("send error");
		}
		memset(buffer, 0, sizeof(buffer));
		exit(EXIT_FAILURE); // Exit on timeout or error
	}

	memset(buffer, 0, sizeof(buffer));
	return true;
}

bool ServerConnection::post_variant(const Variant &variant) {
	String str = json.stringify(variant, "", true, true);

	strcpy(buffer, str.utf8().get_data());
	ssize_t r = send(new_socket, buffer, strlen(buffer), 0);

	if (r == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			perror("send timeout");
		} else {
			perror("send error");
		}
		memset(buffer, 0, sizeof(buffer));
		exit(EXIT_FAILURE); // Exit on timeout or error
	}

	memset(buffer, 0, sizeof(buffer));
	return true;
}
