###############################################################################
# makefile for Project4: Traveling Salesman Problem
# CS325-400-W17	17 Mar 2017
# Jonathan Horton	hortonjo@oregonstate.edu
# Chris Kearns		kearnsc@oregonstate.edu
# Dustin Pack		packdu@oregonstate.edu
###############################################################################
CFLAGS = -Wall -lm -lpthread -std=c++0x

all: TSP.h TSP.cpp Threads.h Threads.cpp main.cpp nNeighbor.cpp
	g++ -o TSP TSP.h TSP.cpp Threads.h Threads.cpp main.cpp $(CFLAGS)
	g++ -o nNeighbor nNeighbor.cpp $(CFLAGS)

# Testing restricted time:
run:
#	TSP example0.txt
#	python tsp-verifier.py example0.txt example0.txt.tour
#	TSP example1.txt
#	python tsp-verifier.py example1.txt example1.txt.tour
#	TSP example2.txt
#	python tsp-verifier.py example2.txt example2.txt.tour
#	TSP example3.txt
#	python tsp-verifier.py example3.txt example3.txt.tour
#	TSP example4.txt
#	python tsp-verifier.py example4.txt example4.txt.tour
#	TSP example5.txt
#	python tsp-verifier.py example5.txt example5.txt.tour
#	TSP tsp_example_1.txt
#	python tsp-verifier.py tsp_example_1.txt tsp_example_1.txt.tour
#	TSP tsp_example_2.txt
#	python tsp-verifier.py tsp_example_2.txt tsp_example_2.txt.tour
#	TSP tsp_example_3.txt
#	python tsp-verifier.py tsp_example_3.txt tsp_example_3.txt.tour
#	TSP test-input-1.txt
#	python tsp-verifier.py test-input-1.txt test-input-1.txt.tour
#	TSP test-input-2.txt
#	python tsp-verifier.py test-input-2.txt test-input-2.txt.tour
#	TSP test-input-3.txt
#	python tsp-verifier.py test-input-3.txt test-input-3.txt.tour
#	TSP test-input-4.txt
#	python tsp-verifier.py test-input-4.txt test-input-4.txt.tour
#	TSP test-input-5.txt
#	python tsp-verifier.py test-input-5.txt test-input-5.txt.tour
#	TSP test-input-6.txt
#	python tsp-verifier.py test-input-6.txt test-input-6.txt.tour
#	TSP test-input-7.txt
#	python tsp-verifier.py test-input-7.txt test-input-7.txt.tour

# Testing unrestricted time:
run1:
#	TSP tsp_example_1.txt 1
#	python tsp-verifier.py tsp_example_1.txt tsp_example_1.txt.tour
#	TSP tsp_example_2.txt 1
#	python tsp-verifier.py tsp_example_2.txt tsp_example_2.txt.tour
#	TSP tsp_example_3.txt 1
#	python tsp-verifier.py tsp_example_3.txt tsp_example_3.txt.tour
#	TSP test-input-1.txt 1
#	python tsp-verifier.py test-input-1.txt test-input-1.txt.tour
#	TSP test-input-2.txt 1
#	python tsp-verifier.py test-input-2.txt test-input-2.txt.tour
#	TSP test-input-3.txt 1
#	python tsp-verifier.py test-input-3.txt test-input-3.txt.tour
#	TSP test-input-4.txt 1
#	python tsp-verifier.py test-input-4.txt test-input-4.txt.tour
#	TSP test-input-5.txt 1
#	python tsp-verifier.py test-input-5.txt test-input-5.txt.tour
#	TSP test-input-6.txt 1
#	python tsp-verifier.py test-input-6.txt test-input-6.txt.tour
#	TSP test-input-7.txt 1
#	python tsp-verifier.py test-input-7.txt test-input-7.txt.tour

clean:
	rm -f TSP nNeighbor *.tour