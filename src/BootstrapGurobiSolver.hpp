#ifndef _FHE_BOOT_GUROBI_SOLVER
#define _FHE_BOOT_GUROBI_SOLVER

#include "BootstrapSolver.hpp"
#include "gurobi_c++.h"
#include "misc.hpp"

class BootstrapGurobiSolver : public BootstrapSolver
{
private:
    GRBEnv env;
    mytimer::timer timer = mytimer::timer();

public:
    BootstrapGurobiSolver() : env("logs/experiment.log"){};
    ProblemResult solve(const int l, const int n, const CircuitGraph &graph,
                        const int max_threads = 0) override;
};

#endif // _FHE_BOOT_GUROBI_SOLVER
