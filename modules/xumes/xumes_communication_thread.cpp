//
// Created by vincent on 15/02/24.
//

#include "xumes_communication_thread.h"





void XumesCommunicationThread::thread_func(void *p_udata) {
	GameServiceImpl comm;

	comm.run(8080);
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

XumesCommunicationThread *XumesCommunicationThread::singleton = NULL;

XumesCommunicationThread *XumesCommunicationThread::get_singleton() {
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

void XumesCommunicationThread::_bind_methods() {}

XumesCommunicationThread::XumesCommunicationThread() {
	singleton = this;
}

