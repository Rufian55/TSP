/****************************************************
 * CS 325 Project 4: Traveling Salesman Problem
 * Jonathan Horton / Chris Kearns / Dustin Pack
 * Created: 2017-03-04
 * Updated: 2017-03-14
 *
 * This program will read in a file that consists
 * of city codes (in ascending integer order from
 * 0 to the number of cities), an X coordinate,
 * and a Y coordinate. These values refer to the
 * positions of the cities in a 2-dimensional grid.
 * The program will then approximate an optimal
 * path that will go through every city once.
 ****************************************************/

#include <iostream>	// for general input/output
#include <fstream>	// for reading file
#include <vector>	// for storage of file
#include <math.h>	// for distance calculation math
#include <string.h>	// for filenames
#include <climits>	// for MAX_INT and such
#include <ctime>	// for timing tests
using std::cout;
using std::endl;
using std::vector;


/*************************************************
 *	readFile
 * This function will read in the given filename
 * and fill the entries in the given vectors.
 * Returns 0 on success and -1 on failure.
 *************************************************/
int readFile(vector<int>& X, vector<int>& Y, std::string fileName) {	
	// Open the file
	std::ifstream inputFile;
	inputFile.open(fileName);
	if (!inputFile) {
		std::cerr << "ERROR: Cannot open file \"" << fileName << "\"" << endl;
		return -1; // report failure
	}

	int temp;
	// Throw out the city codes since they are in ascending order
	while (inputFile >> temp) {
		inputFile >> temp; // read in X coordinate
		X.push_back(temp);
		inputFile >> temp; // read in Y coordinate
		Y.push_back(temp);
	}

	inputFile.close();
	return 0; // report success
}


/*************************************************
 *	distCalc
 * This will return the distance between given
 * cities as the nearest integer. It will return
 * return a -1 if something is out of range.
 *************************************************/
int distCalc(vector<int>& X, vector<int>& Y, int cityA, int cityB) {
	int temp = 0;
	double squareroot = 0;

	if (cityA < 0 || cityA >= (int)X.size() || cityB < 0 || cityB >= (int)Y.size()) {
		std::cerr << "WARNING: distance calc for invalid city." << endl;
		return -1; // out of range!
	}

	squareroot = sqrt( pow(X[cityA] - X[cityB],2) + pow(Y[cityA] - Y[cityB],2) );

	// Since casting a floating point truncates in C++,
	// we can add 0.5 before casting to round to nearest int!
	temp = (int)(squareroot + 0.5);
	
	return temp;
}


/************************************************
 *	totalDist
 * Computes the total path length when visiting
 * cities in a given order.
 ************************************************/
int totalDist(vector<int>& X, vector<int>& Y, vector<int>& path) {
	int distance = 0;

	 // if our current path has NO cities or ONE city
	if ( path.size() <= 1 )
		return 0;

	// Add distance to the next city to the total distance
	for (int counter = 0; counter < (int)path.size() - 1; counter++) {
		distance += distCalc(X, Y, path[counter], path[counter + 1]);	
	}

	// Don't forget the final last city -> first city!
	distance += distCalc(X, Y, path.back(), path.front());

	return distance;
}


/***********************************************
 *	saveTour
 * Writes the tour length and path vector to a
 * file, named after the input file + ".tour".
 * NOTE: Set to overwrite last tour file.
 ***********************************************/
int saveTour(vector<int>& X, vector<int>& Y, vector<int>& path, std::string fileName) {
	std::ofstream fout;
	fout.open(fileName + ".tour"); // open file
	if (!fout) {
		std::cerr << "ERROR: Cannot write to file \"" << fileName << ".tour\"" << endl;
		return -1; // report failure
	}

	// First output the total length
	fout << totalDist(X, Y, path) << endl;

	// Now output the path
	for (int counter = 0; counter < (int)path.size(); counter++)
		fout << path[counter] << endl;

	fout.close(); // close file
	return 0; // report success
}


/*****************************************
 *	removeValue
 * Removes an entry from a given vector.
 *****************************************/
int removeValue(vector<int>& vect, int target) {
	for (int counter = 0; counter < (int)vect.size(); counter++ )
		if (vect[counter] == target) {
			vect.erase (vect.begin() + counter); // remove the value!
			return 0; // report success
		}

	return -1; // report failure
}


/******************************************************
 *	nearestNeighbor -- ALGORITHM 1
 * This is an algorithm that calculates a path for a
 * traveling salesman. It fills the "path" vector and
 * starts with the given city number.
 ******************************************************/
int nearestNeighbor(vector<int>& X, vector<int>& Y, vector<int>& path, int start) {
	int nextDist, nextCity;
	vector<int> toVisit;

	// Create a list of cities we need to visit still
	for (int counter = 0; counter < (int)X.size(); counter++)
		toVisit.push_back(counter);

	// Let's start at the starting position given
	path.push_back(start);
	removeValue(toVisit, start);

	// While we still have cities left in the list to visit... 
	while ( !toVisit.empty() ) {
		nextDist = INT_MAX;
		nextCity = -1;

		for (int counter = 0; counter < (int)toVisit.size(); counter++) {
			// Find the nearest neighbor among cities left to visit!
			if ( 	distCalc(X, Y, path[path.size() - 1], toVisit[counter]) < nextDist ) {
				nextDist = distCalc(X, Y, path[path.size() - 1], toVisit[counter]);
				nextCity = toVisit[counter];
			}
		}
		
		// Add the nearest unvisited neighbor to the path!
		path.push_back(nextCity);
		removeValue(toVisit, nextCity);
	}

	if ( path.size() != X.size() ) {
		std::cerr << "ERROR: Calculated path incomplete" << endl;
		return -1; // report failure to visit all cities
	}

	return 0; // report success
}


/***************
 *	main
 ***************/
int main(int argc, char** argv) {
	vector<int> cityX, cityY; 	// store city coordinates
	vector<int> path; 		// store path traveling to cities
	vector<int> newPath;
	bool infTime = false;

	// Check usage
	if (argc < 2 || argc > 4) {
		cout << "USAGE: " << argv[0] << " [inputFile] [optional int 1]" << endl;
		return 1;
	} else if (argc == 3) {
		if (atoi(argv[2]) < 1 || atoi(argv[2]) > 1) {
			cout << "Optional argument must be 1 or omitted!" << endl;
			return 1;		
		} else {
			infTime = true;
		}
	}
	
	// Convert the filename to a string to keep track of it.
	std::string fileName = argv[1];	

	// Read in the file
	if (readFile(cityX, cityY, fileName) == -1) {
		cout << "File processing error!" << endl;
		return 1;
	}

	// Start a timer for algorithm testing
	std::clock_t start;
	double duration = 0;
	start = std::clock();


	/****************************
	 * ALGORITHM IS CALLED HERE
	 ****************************/

	// Calling Nearest Neighbor Algorithm with a start point of city 0
	if ( nearestNeighbor(cityX, cityY, path, 0 ) == -1 ) {
		cout << "Algorithm 1 error!" << endl;
		return 1;
	}

	// Now we try more starting points until we use all the cities or run out of time
	int cityCounter = 1, savedCity = 0;
	while ( (duration < 90 || infTime) && cityCounter < (int)cityX.size() ) {
		// Populate a new path
		if ( nearestNeighbor(cityX, cityY, newPath, cityCounter) ) {
			cout << "Algorithm 1 error!" << endl;
			return 1;
		}

		// If new path is better than old path, switch them!
		if ( totalDist(cityX, cityY, newPath) < totalDist(cityX, cityY, path) ) {
			path.swap(newPath);
			savedCity = cityCounter;
		}

		// update time elapsed and prepare for next city
		duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		cityCounter++;
		newPath.clear();
	}
	



	// Display time elapsed
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
	cout << "Processing time: " << duration << endl;
	cout << "Cities checked : " << cityCounter << endl;

	// Display length of path found
	cout << "First city is num: " << savedCity << endl;
	cout << "Total path length: " << totalDist(cityX, cityY, path) << endl;

	// Save the tour as a file!
	if ( saveTour(cityX, cityY, path, argv[1]) == 0 )
		cout << "File \"" << fileName << ".tour\" created successfully!" << endl;
	else
		cout << "File output error!" << endl;

	return 0;
}
