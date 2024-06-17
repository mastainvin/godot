#include "xumes_communication_thread.h"
#include "xumes_communication.h"
#include "scenario_runner.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



void XumesCommunicationThread::thread_func(void* p_udata) {
	if (!XumesCommunicationThread::is_port_taken(8080)) {
		XumesCommunication::get_instance()->run(8080);
	}
}

bool XumesCommunicationThread::is_port_taken(int port) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return false; // Si la création du socket échoue, on ne peut pas déterminer si le port est pris ou non
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int bind_result = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));

	close(sockfd);

	return bind_result != 0 ;
}

Error XumesCommunicationThread::init() {
	if (!started) {
		thread_exited = false;
		mutex = new Mutex();
		thread = new Thread();
		thread->start(XumesCommunicationThread::thread_func, this);
		started = true;
	}
	return OK;
}

XumesCommunicationThread* XumesCommunicationThread::singleton = nullptr;

XumesCommunicationThread* XumesCommunicationThread::get_singleton() {
	if (singleton == nullptr) {
		singleton = new XumesCommunicationThread();
	}
	return singleton;
}

void XumesCommunicationThread::unlock() {
	if (!thread || !mutex) {
		return;
	}
	mutex->unlock();
}

void XumesCommunicationThread::lock() {
	if (!thread || !mutex) {
		return;
	}
	mutex->lock();
}

void XumesCommunicationThread::finish() {
	XumesCommunication::get_instance()->close();
	if (!thread) {
		return;
	}
	exit_thread = true;
	memdelete(thread);
	if (mutex) {
		memdelete(mutex);
	}
	thread = nullptr;
}

void XumesCommunicationThread::_bind_methods() {}

XumesCommunicationThread::XumesCommunicationThread() {
	singleton = this;
}

XumesCommunicationThread::~XumesCommunicationThread() {
	if (thread) {
		finish();
	}
}
