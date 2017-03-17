/*******************************************************************************
* TSP.h is the header file for TSP.cpp
* CS-325-400-W17	Project 4		17 March 2017
* Jonathan Horton	hortonjo@oregonstate.edu
* Chris Kearns		kearnsc@oregonstate.edu
* Dustin Pack		packdu@oregonstate.edu
*******************************************************************************/
#ifndef TSP_H
#define TSP_H

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
using std::ifstream;
using std::ios;
using std::ofstream;
#include <iomanip>
#include <iostream>
using std::cout;
#include <limits>
#include <pthread.h>
#include <queue>
#include <stack>
using std::stack;
#include <string>
using std::string;
#include <stdio.h>
#include <vector>
using std::vector;

#define NUM_THREADS 6
#define RUN_SHORTER 5

class TSP {
private:
	struct City {
		int X;
		int Y;
	};
	string inFileName;
	string outFileName;
	vector<int>oddNodes;
	void findOddNodes();
	int nearestNextNode(int *index, bool *MSTBuffer);
public:
	TSP(string inFile, string outFile);
	~TSP();
	// Variables and containers:
	int V;
	vector<City>cities;
	int **graph;
	int pathLength;
	vector<int>circuit;
	vector<int> *adjacencyList;
	int beginIndex[NUM_THREADS];
	int endIndex[NUM_THREADS];
	int **allPathLengths;
	// Methods:
	int calcDistance(struct City city1, struct City city2);
	void readCityData();
	void loadMatrix(int threadGaurd);
	void findMST();
	void matchMST();
	int findBestPath(int index);
	void tourGen(int index);
	void eulerCircuit(int index, vector<int> &path);
	void hamiltonCircuit(vector<int> &path, int&pathLength);
	int _2Opt(int **graph, vector<int> &path, int &pathLength, int V);
	bool pathIsShorter(int **graph, int a, int b, int c, int d, int &pathLength);
	void edgeSwap(vector<int> &path, int start, int end, int V);
	void optimize();
	void printResult();
};
#endif