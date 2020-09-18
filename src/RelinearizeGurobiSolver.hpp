#ifndef _FHE_RELIN_GUROBI_SOLVER
#define _FHE_RELIN_GUROBI_SOLVER

#include "RelinearizeSolver.hpp"
#include "gurobi_c++.h"
#include "misc.hpp"

class RelinearizeGurobiSolver : public RelinearizeSolver
{
private:
    GRBEnv env;
    mytimer::timer timer = mytimer::timer();

public:
    RelinearizeGurobiSolver() : env("logs/experiment.log"){};
    ProblemResult solve(const int kr, const int km, const CircuitGraph &graph,
                        const int max_threads = 0) override;
};

#endif // _FHE_RELIN_GUROBI_SOLVER
