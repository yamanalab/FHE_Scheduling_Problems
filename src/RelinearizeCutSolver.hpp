#ifndef _FHE_RELIN_CUT_SOLVER
#define _FHE_RELIN_CUT_SOLVER

#include "RelinearizeSolver.hpp"
#include "misc.hpp"

class RelinearizeCutSolver : public RelinearizeSolver
{
private:
    mytimer::timer timer = mytimer::timer();
    int min_cut = -1;
    int count_and = 0;

public:
    RelinearizeCutSolver(){};
    ProblemResult solve(const int kr, const int km, const CircuitGraph &graph,
                        const int max_threads = 0) override;
    long calcObjective(const int kr, const int km);
};

#endif // _FHE_RELIN_CUT_SOLVER
