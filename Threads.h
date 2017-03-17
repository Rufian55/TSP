/*******************************************************************************
* Threads.h abstract header file for Threads.cpp
* CS-325-400-W17	Project 4		17 March 2017
* Jonathan Horton	hortonjo@oregonstate.edu
* Chris Kearns		kearnsc@oregonstate.edu
* Dustin Pack		packdu@oregonstate.edu
*******************************************************************************/
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