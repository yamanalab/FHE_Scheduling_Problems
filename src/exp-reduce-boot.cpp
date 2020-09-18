#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "BootstrapGurobiSolver.hpp"
#include "CircuitGraph.hpp"
#include "gurobi_c++.h"
#include "misc.hpp"

using namespace std;

string fill_string(const string &str, int sz)
{
    return string(max(0, sz - (int)str.size()), ' ') + str;
}

int main(int argc, char *argv[])
{

    // Build argument parser and parse
    const string HELP = "help";
    const string DESCRIPTION = "experiment to solve bootstrap problem";
    const string NUM_THREAD = "num-thread";
    const string NUM_TRIAL = "num-trial";

    cola::parser parser;
    parser.define(HELP, "print this text and exit").alias('h');
    parser.define(NUM_THREAD, "number of thread for gurobi.")
      .alias('j')
      .with_arg<int>(0);
    parser.define(NUM_TRIAL, "number of trial for measuring time.")
      .alias('n')
      .with_arg<int>(1);

    parser.define("L", "max level").with_arg<int>(2);
    parser.define("N", "noise after bootstrap").with_arg<int>(2);

    parser.parse(argc, argv);

    if (parser.is_passed(HELP))
    {
        parser.easy_usage(DESCRIPTION);
        exit(0);
    }

    int num_trial = parser.get<int>(NUM_TRIAL);
    if (num_trial <= 0)
    {
        cerr << "invalid number of trial" << endl;
        parser.easy_usage(DESCRIPTION);
        exit(-1);
    }

    int num_thread = parser.get<int>(NUM_THREAD);
    if (num_thread < 0)
    {
        cerr << "invalid number of thread" << endl;
        parser.easy_usage(DESCRIPTION);
        exit(-1);
    }

    vector<string> filenames = parser.rest_args();

    // prepare l,n for experiment
    // vector<pair<int,int>> ln = {{2,2}, {20,11}};
    vector<pair<int, int>> ln; // = {{2,2}};
    ln.push_back({parser.get<int>("L"), parser.get<int>("N")});

    BootstrapGurobiSolver ilpSolver;

    for (auto &circuit_filename : filenames)
    {
        CircuitGraph graph(circuit_filename);
        CircuitGraph reduced = graph.reduceSize();

        // double acc_original_optimize_time_ms = 0;
        // double acc_reduced_optimize_time_ms = 0;

        cout << fill_string(circuit_filename, 35) << " : " << fixed
             << setprecision(4);

        for (auto prm : ln)
        {
            int l = prm.first;
            int n = prm.second;
            double org_optimize_time = 0, red_optimize_time = 0;
            for (int tt = 0; tt < num_trial; tt++)
            {
                auto result = ilpSolver.solve(l, n, graph, num_thread);
                org_optimize_time += result.time_ms;
                auto result2 = ilpSolver.solve(l, n, reduced, num_thread);
                red_optimize_time += result2.time_ms;
                if (result.objective_value != result2.objective_value)
                {
                    dbg(result.objective_value, result2.objective_value);
                    exit(-1);
                }
                // assert(result.objective_value == result2.objective_value);
                // cout << result.objective_value << ", ";
            }
            cout << red_optimize_time / org_optimize_time << ", ";
        }

        // cout << "| " << acc_optimize_time_ms / rm.size() / num_trial << ", ";
        // cout << acc_approx_optimize_time_ms / num_trial << ", ";
        cout << "|V|=" << graph.n_gate << ", |E|=" << graph.n_wire;
        cout << ", |V|=" << reduced.n_gate << ", |E|=" << reduced.n_wire
             << endl;
    }

    return 0;
}
