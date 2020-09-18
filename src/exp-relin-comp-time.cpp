#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "CircuitGraph.hpp"
#include "RelinearizeCutSolver.hpp"
#include "RelinearizeGurobiSolver.hpp"
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
    const string DESCRIPTION = "relinearize problem solving experiment code";
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

    // prepare kr,km for experiment
    vector<pair<int, int>> rm = {{10, 1}, {5, 1}, {3, 1}, {2, 1}, {1, 1},
                                 {1, 2},  {1, 3}, {1, 5}, {1, 10}};
    // {1000,1}, {100,1}, {10,1}, {8,1}, {13,2}, {5,1}, {4,1}, {3,1}, {2,1},
    // {3,2}, {101,100}, {100001,100000}, {1,1}, {100,101}, {100000,100001},
    // {2,3}, {1,2}, {1,3}, {1,4}, {1,5}, {2, 13}, {1,10}, {1,100}};

    try
    {
        RelinearizeCutSolver cutSolver;
        RelinearizeGurobiSolver ilpSolver;

        for (auto &circuit_filename : filenames)
        {
            CircuitGraph graph(circuit_filename);

            double acc_approx_optimize_time_ms = 0;
            double acc_optimize_time_ms = 0;

            for (int tt = 0; tt < num_trial; tt++)
            {
                auto result = cutSolver.solve(1, 1, graph);
                acc_approx_optimize_time_ms += result.time_ms;
            }

            cout << fill_string(circuit_filename, 35) << " : " << fixed
                 << setprecision(6);

            for (auto prm : rm)
            {
                int kr = prm.first;
                int km = prm.second;
                double optimize_time = 0;
                for (int tt = 0; tt < num_trial; tt++)
                {
                    auto result = ilpSolver.solve(kr, km, graph, num_thread);
                    optimize_time += result.time_ms;
                }
                cout << optimize_time / num_trial << ", ";
                acc_optimize_time_ms += optimize_time;
            }

            cout << "| " << acc_optimize_time_ms / rm.size() / num_trial
                 << ", ";
            cout << acc_approx_optimize_time_ms / num_trial << ", ";
            cout << "|V|=" << graph.n_gate << ", |E|=" << graph.n_wire << endl;
        }
    }
    catch (GRBException e)
    {
        cout << e.getMessage() << endl;
    }

    return 0;
}
