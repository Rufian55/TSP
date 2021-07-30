#include "Threads.h"

// Called from main() once thread setup and partitioning routines complete.
void Thread::start() {
	pthread_create(&tid, NULL, &Thread::execute, this);
}

// Each thread is run from here. Cannot be called from main() as set private.
void *Thread::execute(void *thread) {
	((Thread *)thread)->runTSPonThisThread();
	return NULL;
}

// Called from Thread::execute() above.
void Thread::runTSPonThisThread() {
	int result = aThread->findBestPath(startNode);
	aThread->allPathLengths[TSPid][0] = startNode;
	aThread->allPathLengths[TSPid][1] = result;
	pthread_exit(0);
}

// Wait for thread termination status. Called from main().
void Thread::join() {
	void *threadStatus;
	int temp = pthread_join(tid, &threadStatus);
	if (temp) {
		cout << "ERROR from tid " << TSPid << ". Error returned from "
			<< "pthread_join() is " << temp << "\n";
		exit(1);
	}
}
