#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include "TSP.h"

class Thread {
private:
	pthread_t tid;
	static void *execute(void *thread);
public:
	Thread() {
		aThread = NULL;
		tid = 0;
		TSPid = -1;
		startNode = -1;
	};
	virtual ~Thread() {};
	virtual void runTSPonThisThread();
	void start();
	void join();
	TSP *aThread;
	int startNode;
	int TSPid;
};
#endif