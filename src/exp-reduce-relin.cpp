#include "gurobi_c++.h"
#include "misc.hpp"

#include "CircuitGraph.hpp"
#include "RelinearizeGurobiSolver.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace std;

string fill_string(const string &str, int sz) {
  return string(max(0, sz - (int)str.size()), ' ') + str;
}

int main(int argc, char *argv[]) {

  // Build argument parser and parse
  const string HELP = "help";
  const string DESCRIPTION = "relinearize problem solving experiment code";
  const string NUM_THREAD = "num-thread";
  const string NUM_TRIAL = "num-trial";

  cola::parser parser;
  parser.define(HELP, "print this text and exit")
    .alias('h');
  parser.define(NUM_THREAD, "number of thread for gurobi.")
    .alias('j').with_arg<int>(0);
  parser.define(NUM_TRIAL, "number of trial for measuring time.")
    .alias('n').with_arg<int>(1);

  parser.parse(argc, argv);

  if (parser.is_passed(HELP)) {
    parser.easy_usage(DESCRIPTION);
    exit(0);
  }

  int num_trial = parser.get<int>(NUM_TRIAL);
  if (num_trial <= 0) {
    cerr << "invalid number of trial" << endl;
    parser.easy_usage(DESCRIPTION);
    exit(-1);
  }

  int num_thread = parser.get<int>(NUM_THREAD);
  if (num_thread < 0) {
    cerr << "invalid number of thread" << endl;
    parser.easy_usage(DESCRIPTION);
    exit(-1);
  }

  vector<string> filenames = parser.rest_args();

  // prepare kr,km for experiment
  vector<pair<int,int>> rm = {
    {10,1}, {5,1}, {3,1}, {2,1}, {1,1}, {1,2}, {1,3}, {1,5}, {1,10}};
    // {1000,1}, {100,1}, {10,1}, {8,1}, {13,2}, {5,1}, {4,1}, {3,1}, {2,1}, {3,2}, {101,100}, {100001,100000},
    // {1,1},
    // {100,101}, {100000,100001}, {2,3}, {1,2}, {1,3}, {1,4}, {1,5}, {2, 13}, {1,10}, {1,100}};

  RelinearizeGurobiSolver ilpSolver;
  // RelinearizeCutSolver cutSolver;

  for(auto &circuit_filename : filenames) {
    CircuitGraph graph(circuit_filename);
    CircuitGraph reduced = graph.reduceSize();

    double acc_original_optimize_time_ms = 0;
    double acc_reduced_optimize_time_ms = 0;

    cout << fill_string(circuit_filename, 35) << " : " << fixed << setprecision(4);

    for(auto prm : rm) {
      int kr = prm.first;
      int km = prm.second;
      double org_optimize_time = 0, red_optimize_time = 0;
      for(int tt=0; tt<num_trial; tt++){
        auto result = ilpSolver.solve(kr, km, graph, num_thread);
        // auto result = cutSolver.solve(kr, km, graph);
        org_optimize_time += result.time_ms;
        auto result2 = ilpSolver.solve(kr, km, reduced, num_thread);
        // auto result2 = cutSolver.solve(kr, km, reduced);
        red_optimize_time += result2.time_ms;
        if(result.objective_value != result2.objective_value) {
          dbg(result.objective_value, result2.objective_value);
          exit(-1);
        }
        // assert(result.objective_value == result2.objective_value);
      }
      cout << red_optimize_time / org_optimize_time << ", ";
    }

    // cout << "| " << acc_optimize_time_ms / rm.size() / num_trial << ", ";
    // cout << acc_approx_optimize_time_ms / num_trial << ", ";
    cout << "|V|=" << graph.n_gate << ", |E|=" << graph.n_wire;
    cout << ", |V|=" << reduced.n_gate << ", |E|=" << reduced.n_wire << endl;
  }

  return 0;
}
