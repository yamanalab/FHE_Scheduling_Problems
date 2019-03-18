#ifndef _FHE_BOOT_SOLVER
#define _FHE_BOOT_SOLVER

#include "CircuitGraph.hpp"

class ProblemResult {
public:
  long objective_value;
  double time_ms;
  ProblemResult(const long obj, const double ms) : objective_value(obj), time_ms(ms) {};
};

class BootstrapSolver {
public:
  virtual ProblemResult solve(const int l, const int n, const CircuitGraph &graph, const int max_threads) = 0;
};

#endif // _FHE_BOOT_SOLVER
