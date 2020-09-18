#ifndef _FHE_RELIN_SOLVER
#define _FHE_RELIN_SOLVER

#include "CircuitGraph.hpp"

class ProblemResult
{
public:
    long objective_value;
    double time_ms;
    ProblemResult(const long obj, const double ms)
      : objective_value(obj), time_ms(ms){};
};

class RelinearizeSolver
{
public:
    virtual ProblemResult solve(const int kr, const int km,
                                const CircuitGraph &graph,
                                const int max_threads) = 0;
};

#endif // _FHE_RELIN_SOLVER
