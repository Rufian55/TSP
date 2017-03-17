README.txt

CS-325-400-W17	Project 4  17 March 2017
Jonathan Horton	hortonjo@oregonstate.edu
Chris Kearns	kearnsc@oregonstate.edu

Dustin Pack	packdu@oregonstate.edu

Instructions:
Unzip to a clean directory of your choice.
Type "make".
TSP <input_file.txt>        (will run TSP with time throttling)
TSP <input_file.txt> 1      (will run TSP with time throttling overridden)

Zip file payload:

README.txt   (this file)
makefile
TSP.h
TSP.cpp
Threads.h
Threads.cpp
main.cpp
nNeighbor.cpp (our alternative implementation, not used in the *.tour file generation).

// *.tour files.
test-input-1.txt.tour
test-input-2.txt.tour
test-input-3.txt.tour
test-input-4.txt.tour
test-input-5.txt.tour
test-input-6.txt.tour
test-input-7.txt.tour
tsp_example_1.txt.tour
tsp_example_2.txt.tour
tsp_example_3.txt.tour

// Input files, including base cases n = 0, 1, 2, 3, 4, and 5.
// Base cases:
example0.txt
example1.txt
example2.txt
example3.txt
example4.txt
example5.txt

// Known optimal solution cases:
tsp_example_1.txt
tsp_example_2.txt
tsp_example_3.txt

// Competition cases:
test-input-1.txt
test-input-2.txt
test-input-3.txt
test-input-4.txt
test-input-5.txt
test-input-6.txt
test-input-7.txt

// Verification python files:
TSPAllVisited.py
tsp-verifier.py
watch.py	(set to 3 minutes)

Base case n = 0 and n = 2 stdout correct information but do not generate .tour files.
Base case n = 1 and n < the threadGuard variable, as set, n = 3, 4, and 5 do generate
    .tour files, however, we omit these .tour files as unintersting.
Our base case example?.txt files are included for your convenience, as nescessary.
