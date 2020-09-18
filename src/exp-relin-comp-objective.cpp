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
    const string PARAM_KR = "kr";
    const string PARAM_KM = "km";
    const string DESCRIPTION = "relinearize problem solving experiment code";

    cola::parser parser;
    parser.define(HELP, "print this text and exit").alias('h');
    parser.define(PARAM_KR, "k_r of relinearize problem")
      .alias('r')
      .with_arg<int>();
    parser.define(PARAM_KM, "k_m of relinearize problem")
      .alias('m')
      .with_arg<int>();
    parser.parse(argc, argv);

    if (parser.is_passed(HELP))
    {
        parser.easy_usage(DESCRIPTION);
        exit(0);
    }

    vector<string> filenames = parser.rest_args();

    // prepare kr,km for experiment
    vector<pair<int, int>> rm = {{10, 1}, {5, 1}, {3, 1}, {2, 1}, {1, 1}};
    // {1000,1}, {100,1}, {10,1}, {8,1}, {13,2}, {5,1}, {4,1}, {3,1}, {2,1},
    // {3,2}, {101,100}, {100001,100000}, {1,1}, {100,101}, {100000,100001},
    // {2,3}, {1,2}, {1,3}, {1,4}, {1,5}, {2, 13}, {1,10}, {1,100}};

    if (parser.is_passed(PARAM_KM) && parser.is_passed(PARAM_KR))
    {
        rm = {{parser.get<int>(PARAM_KM), parser.get<int>(PARAM_KR)}};
    }

    RelinearizeCutSolver cutSolver;
    RelinearizeGurobiSolver ilpSolver;

    for (auto &circuit_filename : filenames)
    {
        CircuitGraph graph(circuit_filename);

        cutSolver.solve(1, 1, graph);

        cout << fill_string(circuit_filename, 35) << " : " << fixed
             << setprecision(6);

        for (auto prm : rm)
        {
            int kr = prm.first;
            int km = prm.second;
            auto optimalResult = ilpSolver.solve(kr, km, graph, 0);
            long optimalObjective = optimalResult.objective_value;
            long approxObjective = cutSolver.calcObjective(kr, km);

            if (optimalObjective != approxObjective)
            {
                cout << ioscc::green;
            }
            else
            {
                cout << ioscc::_sesc << ioscc::_sreset;
            }
            cout << approxObjective << "/" << optimalObjective;
            cout << "(" << (double)approxObjective / optimalObjective << "), ";
            if (!(kr > km || optimalObjective == approxObjective))
            {
                dbg(kr, km, optimalObjective, approxObjective);
                assert(false);
            }
        }

        cout << endl;
    }

    return 0;
}
