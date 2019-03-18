CC = g++-5

CFLAGS = -O2 -std=c++14 -Wall -Wshadow -g -I$(GUROBI_HOME)/include/
LDLIBS = -L$(GUROBI_HOME)/lib/ -lgurobi_g++5.2 -lgurobi80 -lm

HEADERS = CircuitGraph.hpp MaximumFlow.hpp RelinearizeSolver.hpp RelinearizeCutSolver.hpp RelinearizeGurobiSolver.hpp

all: relinCompTime.out relinCompObj.out reduceRelin.out reduceBoot.out

relinCompTime.out: exp-relin-comp-time.cpp CircuitGraph.o RelinearizeCutSolver.o RelinearizeGurobiSolver.o
	$(CC) $(CFLAGS) -o relinCompTime.out exp-relin-comp-time.cpp CircuitGraph.o RelinearizeCutSolver.o RelinearizeGurobiSolver.o $(LDLIBS)

relinCompObj.out: exp-relin-comp-objective.cpp CircuitGraph.o RelinearizeCutSolver.o RelinearizeGurobiSolver.o
	$(CC) $(CFLAGS) -o relinCompObj.out exp-relin-comp-objective.cpp CircuitGraph.o RelinearizeCutSolver.o RelinearizeGurobiSolver.o $(LDLIBS)

reduceRelin.out: exp-reduce-relin.cpp CircuitGraph.o RelinearizeGurobiSolver.o
	$(CC) $(CFLAGS) -o reduceRelin.out exp-reduce-relin.cpp CircuitGraph.o RelinearizeGurobiSolver.o $(LDLIBS)

reduceBoot.out: exp-reduce-boot.cpp CircuitGraph.o BootstrapGurobiSolver.o
	$(CC) $(CFLAGS) -o reduceBoot.out exp-reduce-boot.cpp CircuitGraph.o BootstrapGurobiSolver.o $(LDLIBS)


%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.out
