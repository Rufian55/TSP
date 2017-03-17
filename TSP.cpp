/*******************************************************************************
* TSP.cpp is the implementation file for TSP.h
* CS-325-400-W17	Project 4		17 March 2017
* Jonathan Horton	hortonjo@oregonstate.edu
* Chris Kearns		kearnsc@oregonstate.edu
* Dustin Pack		packdu@oregonstate.edu
*******************************************************************************/
#include "TSP.h"

// TSP Class constructor.
TSP::TSP(string inFile, string outFile) {
	inFileName = inFile;
	outFileName = outFile;
	int numLines = 0;
	ifstream inStream;

	// Get nodes V(city) count in input file and updates TSP::V.
	inStream.open(inFileName.c_str(), ios::in);

	if (!inStream) {
		cout << "Error: TSP.cpp failed to open input file " << inFileName << "\n";
		exit(1);
	}
	string dummy;
	while (std::getline(inStream, dummy))
		numLines++;
	V = numLines;
	inStream.close();

	// Allocate memory and initialize graph.
	graph = new int*[V];
	for (int i = 0; i < V; i++) {
		graph[i] = new int[V];
		for (int j = 0; j < V; j++)
			graph[i][j] = 0;
	}

	allPathLengths = new int*[V];
	for (int i = 0; i < V; i++) {
		allPathLengths[i] = new int[V];
	}

	adjacencyList = new vector<int>[V];
};

// TSP Class destructor (memory manager).
TSP::~TSP() {
	for (int i = 0; i < V; i++) {
		delete[] graph[i];
		delete[] allPathLengths[i];
	}
	delete[] allPathLengths;
	delete[] graph;
	delete[] adjacencyList;
}

/* Reads X and Y coordinates from input file and updates cities vector.  Assumes input
   file is sorted by city int as cityNum is discarded here. */
void TSP::readCityData() {
	ifstream inStream;
	inStream.open(inFileName.c_str(), ios::in);
	if (!inStream) {
		cout << "Error: TSP.cpp failed to open input file " << inFileName << "\n";
		exit(1);
	}
	int cityNum, X, Y;
	while (!inStream.eof()) {
		inStream >> cityNum >> X >> Y; // cityNum discarded.
		struct City city = { X, Y };
		cities.push_back(city);
	}
	inStream.close();
};

// Returns calculated distance given two pairs of city x|y coordinates.
int TSP::calcDistance(struct TSP::City cityA, struct TSP::City cityB) {
	return round(sqrt(pow(cityA.X - cityB.X, 2) + pow(cityA.Y - cityB.Y, 2)));
};

// Utility struct used for managing thread assignment to instances of TSP class.
struct threadData { int tid; TSP *tsp; };
struct threadData *data;

// Populates the graph matrix with calculated distances.  Called by loadMatrix().
void *populateMatrix(void* args) {
	struct threadData *_data = (struct threadData *) args;
	int tid = _data->tid;
	TSP *tsp = _data->tsp;
	int **graph = tsp->graph;
	int begin, end;

	begin = tsp->beginIndex[tid];
	end = tsp->endIndex[tid];

	// Fill matrix with ALL edge distances.
	for (int i = begin; i <= end; i++) {
		for (int j = i; j < tsp->V; j++) {
			graph[i][j] = graph[j][i] = tsp->calcDistance(tsp->cities[i], tsp->cities[j]);
		}
	}
	pthread_exit(NULL);
}

// Populates graph matrix via threading routines. Runs O(~n^3) [look at populateMatrix].
void TSP::loadMatrix(int threadGuard) {
	int someThreads = (V / threadGuard) * (double)(1 / threadGuard);
	int temp = (V / threadGuard) - someThreads;
	int remaining = V - (temp * threadGuard);
	int theRest = threadGuard / 2 + 1;

	int index = 0;
	for (int i = 0; i < theRest; i++) {
		beginIndex[i] = index;
		index += (temp - 1);
		endIndex[i] = index;
		index++;
	}
	int remainder = threadGuard - theRest + 1;
	int give2each = remaining / remainder;

	// Distribute remaining threads.
	for (int i = theRest; i < threadGuard; i++) {
		beginIndex[i] = index;
		index += (temp + give2each - 1);
		endIndex[i] = index;
		index++;
	}
	endIndex[threadGuard - 1] = V - 1;

	int rc;
	void *threadStatus;
	data = new struct threadData[V];

	// Allocate memory for V tid's.
	pthread_t *thread = new pthread_t[V];
	pthread_attr_t attr;

	// Initialize/set thread detached state attribute.
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// Create thread(s).
	for (int i = 0; i < threadGuard; i++) {
		data[i].tid = i;
		data[i].tsp = this;
		rc = pthread_create(&thread[i], &attr, populateMatrix, (void*)&data[i]);
		if (rc) {
			cout << "ERROR: pthread_create() returned: " << rc << "\n";
			exit(1);
		}
	}

	// Destroy threads / wait for other threads to finish.
	pthread_attr_destroy(&attr);
	for (int i = 0; i < threadGuard; i++) {
		rc = pthread_join(thread[i], &threadStatus);
		if (rc) {
			cout << "ERROR: pthread_join() returned error: " << rc << "\n";
			exit(1);
		}
	}
	delete[] data;
	delete[] thread;
}

/* On each iteration, finds a minimum-weight Minimum Spanning Tree
   connecting a vertex v in the set A to the vertex u OUTSIDE of
   set A using an implementation of Prim's algorithm.    */
void TSP::findMST() {
	int index[V];
	bool MSTbuffer[V];
	int parent[V];

	for (int i = 0; i < V; i++) {
		index[i] = INT_MAX;
		MSTbuffer[i] = false;
	}

	// Root nodes.
	index[0] = 0;
	parent[0] = -1;

	// Find nearest node not yet in MST and add it.
	for (int i = 0; i < V-1; i++) {
		int v = nearestNextNode(index, MSTbuffer);
		MSTbuffer[v] = true;
		for (int u = 0; u < V; u++) {
			if (graph[v][u] && MSTbuffer[u] == false && graph[v][u] < index[u]) {
				parent[u] = v;
				index[u] = graph[v][u];
			}
		}
	}
	// Update adjacency lists.
	for (int i = 0; i < V; i++) {
		int j = parent[i];
		if (j != -1) {
			adjacencyList[i].push_back(j);
			adjacencyList[j].push_back(i);
		}
	}
};

// Used by findMST - helper function to find the "nearest neighbor".
int TSP::nearestNextNode(int *index, bool *MSTBuffer) {
	int min = INT_MAX;
	int minIndex;
	for (int v = 0; v < V; v++)
		if (index[v] < min && MSTBuffer[v] != true ) {
			min = index[v];
			minIndex = v;
		}
	return minIndex;
};

/* Finds nodes with odd degree (odd adjacencyList count) and adds to
   oddNodes vector.  Called by matchMST. */
void TSP::findOddNodes() {
	for (int i = 0; i < V; i++) {
		if ((adjacencyList[i].size() % 2) != 0) {
			oddNodes.push_back(i);
		}
	}
}

/* Finds "perfect matching" MST in the subgraph O using a greedy 
   strategy, thus not nescessarily a minimum. Called from main(). */
void TSP::matchMST() {
	int closestNode;
	int length;
	vector<int>::iterator temp;
	vector<int>::iterator first;

	findOddNodes();

	while (!oddNodes.empty()) {
		first = oddNodes.begin();
		vector<int>::iterator iter;
		vector<int>::iterator end = oddNodes.end();
		length = INT_MAX;
		for (iter = oddNodes.begin()+1; iter != end; iter++) {
			/* If iterated node is closer than closestNode,
				update closestNode and length.*/
			if (graph[*first][*iter] < length) {
				length = graph[*first][*iter];
				closestNode = *iter;
				temp = iter;
			}
		}
		// Update adjacencyList and oddNodes.
		adjacencyList[*first].push_back(closestNode);
		adjacencyList[closestNode].push_back(*first);
		oddNodes.erase(temp);
		oddNodes.erase(first);
	}
}

/* Determines a Euler circuit [uses every edge of a graph exactly 
   once] from the inputed vector path and index.  Updates vector
   path in place. */
void TSP::eulerCircuit(int index, vector<int> &path) {
	// Instantiate temp copy of original adjcencyList.
	vector<int> *temp = new vector<int>[V];
	// Resize temp to match adjacencyList and assign.
	for (int i = 0; i < V; i++) {
		temp[i].resize(adjacencyList[i].size());
		temp[i] = adjacencyList[i];
	}
	// Since path is passed by reference:
	path.clear();

	// Utility stack.
	stack<int> stack;
	// Repeat until current vertex neighbors == 0 & stack empty.
	while (temp[index].size() > 0 || !stack.empty()) {
		// If current vertex has no neighbors
		if (temp[index].size() == 0) {
			path.push_back(index);
			int last = stack.top();
			stack.pop();
			index = last;
		}
		else {
			stack.push(index);
			int next2 = temp[index].back();
			temp[index].pop_back();
			for (unsigned int i = 0; i < temp[next2].size(); i++)
				if (temp[next2][i] == index) {
					temp[next2].erase(temp[next2].begin() + i);
					break;
				}
			index = next2;
		}
	}// Get the last leg of the journey.
	path.push_back(index);
	delete[] temp;
}

/* Detemines a Hamiltonian circuit [visits each vertex exactly once] from the
   previoulsy constructed Euler circuit. [See eulerCircuit() above.] Called from 
   tourGen(). Int pathLength is updated by reference. */
void TSP::hamiltonCircuit(vector<int> &path, int &pathLength) {
	bool visited[V];
	for (int i = 0; i < V; i++) {
		visited[i] = false;
	}
	pathLength = 0;
	int startAt = path.front();
	vector<int>::iterator currNode = path.begin();
	vector<int>::iterator nextNode = path.begin() + 1;
	visited[startAt] = true;

	while (nextNode != path.end()) {
		if (!visited[*nextNode]) {
			pathLength += graph[*currNode][*nextNode];
			currNode = nextNode;
			visited[*currNode] = true;
			nextNode = currNode + 1;
		}
		else {
			nextNode = path.erase(nextNode);
		}
	}// Get the last leg of the journey.
	pathLength += graph[*currNode][*nextNode];
}

/* Calls eulerCircuit() by passing circuit vector then converts 
   resulting euler circuit to a Hamiltonian circuit by updating
   circuit vector. */
void TSP::tourGen(int index) {
	eulerCircuit(index, circuit);
	hamiltonCircuit(circuit, pathLength);
}

/* Calls eulerCircuit() and hamiltonCircuit() [but uses a utiity vector
   path for updating], thus finds a path length from a specified
   node and returns it.  Called by Threads::run().     */
int TSP::findBestPath(int index) {
	vector<int>path;
	eulerCircuit(index, path);

	int length;
	hamiltonCircuit(path, length);

	for (int i = 0; i < RUN_SHORTER; i++) {
		_2Opt(graph, path, length, V);
	}
	return length;
}

// Runs _2Opt RUN_SHORTER times.
void TSP::optimize() {
	for (int i = 0; i < RUN_SHORTER; i++) {
		_2Opt(graph, circuit, pathLength, V);
	}
}

/* Swaps edges by removing edge 1 and edge 2 and reconnecting using
   a new path. Begin and end are edge 1's v and edge 2's u.  Called 
   from _2Opt() only and works with pathIsShorter(). */
void TSP::edgeSwap(vector<int> &path, int begin, int end, int V) {
	while (end - begin > 0) {
		int temp = path[begin % V];
		path[begin % V] = path[end % V];
		path[end % V] = temp;
		begin++;
		end--;
	}
}

/* Returns appropriate bool if a shorter path between vertices is found and updates 
   pathLength int as nescessary. Returns true if crossover condition exists.*/
bool TSP::pathIsShorter(int **graph, int a, int b, int c, int d, int &pathLength) {
	int dist1 = graph[a][b]; // edge path old.
	int dist2 = graph[c][d]; // edge path old.
	int dist3 = graph[c][a]; // edge path new.
	int dist4 = graph[d][b]; // edge path new.
	// If shorter, update int pathLength.
	if ((dist1 + dist2) < (dist3 + dist4)) {
		pathLength -= (dist3 + dist4 - dist1 - dist2);
		return true;
	}
	return false;
}

/* Takes a valid path and tests for shorter paths (ones that cross over each other)
   and reorders the path.  Utilizes helper functions pathIsShorter() and edgeSwap().*/
int TSP::_2Opt(int **graph, vector<int> &path, int &pathLength, int V) {
	int count = 0;
	int priorDistance = pathLength;
	int a, b; // First edge path pair.
	int c, d; // Second edge path pair.

	for (int i = 0; i < V; i++) {
		a = (i + 1) % V;
		c = i;
		for (int j = i+2; (j+1) % V != i; j++) {
			b = (j+1) % V;
			d = j % V;
			if (pathIsShorter(graph, path[a], path[b], path[c], path[d], pathLength)) {
				edgeSwap(path, i+1, j, V);
				if ((count == RUN_SHORTER) && (pathLength - priorDistance < RUN_SHORTER)) {
					break;
				}
				if (pathLength - priorDistance > RUN_SHORTER) {
					i = 0;
				}
				priorDistance = pathLength;
				count++;
			}
		}
	}
	return pathLength;
}

/* Writes the "optimal" pathLength and inorder circuit to outFileName.  */
void TSP::printResult() {
	ofstream outputStream;
	outputStream.open(outFileName.c_str(), ios::out);
	outputStream << pathLength << "\n";
	for (vector<int>::iterator iter = circuit.begin(); iter != circuit.end(); iter++) {
		outputStream << *iter << "\n";
	}
	outputStream.close();
};