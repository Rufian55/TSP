/*******************************************************************************
* TSP is an implementation of the approximation solution for the special case
* Euclidian Distance Traveling Salesman Problem utilizing the Christofides
* algorithm [10]:
*  Create a minimum spanning tree T of complete undirected graph G.
*  Let O be the set of vertices with odd degree in T. Set O will have an even
   number of vertices.
*  Find a minimum-weight perfect matching M in the induced subgraph given by
   the vertices from O.
*  Combine the edges of M and T to form a connected multigraph H in which each
   vertex has even degree.
*  Form an Eulerian circuit in H.
*  Make the Eulerian circuit into a Hamiltonian circuit by skipping repeated
   vertices (shortcutting).
* main.cpp, dependent on Threads.h, Threads.cpp, TSP.h, and TSP.cpp
* CS-325-400-W17	Project 4		17 March 2017
* Jonathan Horton	hortonjo@oregonstate.edu
* Chris Kearns		kearnsc@oregonstate.edu
* Dustin Pack		packdu@oregonstate.edu
* Requires input file 3 x n, space delmited, int city, int x-coor, int y-coor.
*******************************************************************************/
#include <climits>
#include <time.h>
#include "TSP.h"
#include "Threads.h"

int main(int argc, char** argv) {

	bool incVar_overRide = false;

	if (argc < 2 || argc > 4) {
		cout << "Usage: ./TSP [inputfile] [optional int 1]\n"
			<< "TSP requires Euclidian distance calculations, so\n"
			<< "inputfile format is space delimited INTEGERS like:\n"
			<< "city0 X-coordinate Y-coordinate\n"
			<< "city1 X-coordinate Y-coordinate\n"
			<< "with only 3 ints per line & at least 2 lines.\n";
		exit(1);
	}
	else if (argc == 3) {
		if (atoi(argv[2]) < 1 || atoi(argv[2]) > 1) {
			cout << "Optional argument must be 1 or omitted!\n";
			exit(1);
		}
		else {
			incVar_overRide = true;
		}
	}

	struct timespec start, end;
	double elapsed;

	// Start timing
	clock_gettime(CLOCK_MONOTONIC, &start);

	// Read in file names.
	string inFile, outFile;
	inFile = outFile = argv[1];
	outFile.append(".tour");

	// Create new tsp object.
	TSP tsp(inFile, outFile);
	int V = tsp.V;

	// Read cities from input file.
	tsp.readCityData();
	cout << "Number of cities read from file: " << tsp.V << "\n";

	// Base case V == 0, V == 2. V == 1 works!
	int oneStopTrip;
	if (V == 0) {
		cout << "InputFile empty! Please check your file and try again!\n";
		exit(1);
	} else if (V == 2) {// V == 1 works!
		oneStopTrip = tsp.calcDistance(tsp.cities[0], tsp.cities[1]);
		cout << "Just two cities entered, distance = " << oneStopTrip << "\n";
		exit(0);
	}

	// Guard against cities < NUM_THREADS.
	int threadGaurd;
	if (V < NUM_THREADS) {
		threadGaurd = V;
	} else {
		threadGaurd = NUM_THREADS;
	}

	// Populate V x V matrix with calculated edge distances.
	tsp.loadMatrix(threadGaurd);

	// Find an MST in graph using Prim's algorithm.
	tsp.findMST();

	// Find a "perfect" minimum weighted matching MST.
	tsp.matchMST();

	// Create array of thread objects.
	Thread threads[threadGaurd];

	// Thread Management utility variables.
	int pathLength = INT_MAX;
	int bplIndex;
	int terminus = threadGaurd;

	/* This block used to fine tune the execution time of TSP
		when running higher values of input cities vs. the desired
		level of accuracy.  Tested against known optimal solutions.
		Formula developed from fit data targeting ~90 seconds, which 
		is needed as V approaches 15,000.  Once V gets to ~15,000,
		then incVar = V. */

	// Advance starting node incVar for each run by:
	int incVar = 5.18 * pow(10, -10) * pow(V, 3.224) + 0.5;
	if (incVar < 1)
		incVar = 1;
	if (incVar > V) 
		incVar = V;
	if (incVar_overRide == true) {
		incVar = 1;
		cout << "Note: Time required to reach Optimal Solution unrestricted!\nProcessing...\n";
	}
	else {
		cout << "Processing...\n";
	}

	// Start at node:
	int node = 0;

	// Count to get thread ids.
	int count = 0;

	// Thread manager - incrementally advance by (threadGuard * incVar) nodes.
	int spool = V;
	while (spool >= incVar) {
		if (spool < (threadGaurd * incVar)) {
			terminus = spool / incVar;
		}
		for (int i = 0; i < terminus; i++) {
			threads[i].aThread = &tsp;
			threads[i].startNode = node; 
			threads[i].TSPid = count;
			threads[i].start(); // See Thread::start().
			node += incVar;
			count++;
		}
		// Wait for all threads to finish.
		for (int i = 0; i < terminus; i++) {
			threads[i].join();
		}
		spool -= (terminus * incVar);
	}

	cout << "Number of cities processed: " << count << "\n";

	// Find shortest path!
	for (int i = 0; i < count; i++) {
		if (tsp.allPathLengths[i][1] < pathLength) {
			bplIndex = tsp.allPathLengths[i][0];
			pathLength = tsp.allPathLengths[i][1];
		}
	}

	// Store best pathLength.
	tsp.tourGen(bplIndex);

	tsp.optimize();

	// stdout >> *.tour file.
	tsp.printResult();

	// End timer.
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed = (end.tv_sec - start.tv_sec);
	elapsed += (end.tv_nsec - start.tv_nsec) / 1000000000.0;

	cout << "TSP's Best Path Length: " << tsp.pathLength << " when starting at city " << bplIndex << "\n";
	cout << "Time to solution: " << elapsed << " seconds.\n";
	return 0;
}

/* CITATIONS: All code adapted from the following sources:
[1] Various lecture materials, algorithms, and programs, CS325-400-W17, J. Shulfort, et. al. @Oregon State University.
[2] h ttp://man7.org/linux/man-pages/man3/pthread_create.3.html
[3] h ttp://man7.org/linux/man-pages/man3/pthread_join.3.html
[4] h ttp://man7.org/linux/man-pages/man3/strerror.3.html
[5] h ttps://www.tutorialspoint.com/cplusplus/cpp_multithreading.htm
[6] h ttps://www.go4expert.com/articles/writing-multithreaded-program-cpp-t29980/
[7] Various lecture materials and programs, CS344-400-F16, B. Brewster, et. al. @Oregon State University.
[8] "C++ Multithreading Cookbook", 2014, M. Ljumovic.
[9] "C++ Concurrency in Action: Practical Multithreading", 1st Ed., A. Williams.
[10] "C++ Implementation of TSP using Christofides and 2-opt", R. Sagalyn, 2013. 
[11] h ttps://en.wikipedia.org/wiki/Christofides_algorithm
[12] h ttp://www.geeksforgeeks.org/greedy-algorithms-set-5-prims-minimum-spanning-tree-mst-2/
[13] h ttp://www.sanfoundry.com/cpp-program-find-mst-prims-algorithm/
[14] h ttp://www.geeksforgeeks.org/eulerian-path-and-circuit/
[15] h ttp://www.sanfoundry.com/cpp-program-find-hamiltonian-cycle/
[16] h ttp://www.geeksforgeeks.org/travelling-salesman-problem-set-2-approximate-using-mst/
[17] h ttp://www.geeksforgeeks.org/branch-bound-set-5-traveling-salesman-problem/
[18] h ttp://www.geeksforgeeks.org/backtracking-set-7-hamiltonian-cycle/
[19] h ttp://www.technical-recipes.com/2012/applying-c-implementations-of-2-opt-to-travelling-salesman-problems/
[20] h ttp://www.jot.fm/issues/issue_2003_07/column8/
[21] h ttp://stackoverflow.com/questions/2962785/c-using-clock-to-measure-time-in-multi-threaded-programs/2962914#2962914

Optimal tour lengths:
tsp_example_1.txt = 108159
tsp_example_2.txt = 2579
tsp_example_3.txt = 1573084
*/