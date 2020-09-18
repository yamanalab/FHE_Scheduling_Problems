#include "RelinearizeCutSolver.hpp"

#include "MaximumFlow.hpp"
#include "misc.hpp"

ProblemResult RelinearizeCutSolver::solve(const int kr, const int km,
                                          const CircuitGraph &graph,
                                          const int max_threads)
{
    timer.set();

    MaximumFlow<int> flow(2 * graph.n_gate + 2);
    int s = 2 * graph.n_gate, t = s + 1;

    count_and = 0;
    for (int i = 0; i < graph.n_gate; i++)
    {
        if (graph.gates[i].type == AND)
        {
            flow.add_edge(s, 2 * i + 1, 1); // apply just after AND
            flow.add_edge(2 * i, t, 2);     // just befor AND, apply both inputs
            count_and++;
        }
        else
        {
            flow.add_edge(2 * i, 2 * i + 1, 1); // apply intermediately
        }
    }

    for (int i = graph.n_input; i < graph.n_wire - graph.n_output; i++)
    {
        auto e = graph.wires[i];
        for (auto to : e.dests)
        {
            flow.add_edge(2 * e.src + 1, 2 * to, INF);
        }
    }

    // force every output to be back size 2
    for (int i = graph.n_wire - graph.n_output; i < graph.n_wire; i++)
    {
        auto e = graph.wires[i];
        flow.add_edge(2 * e.src + 1, t, INF);
    }

    min_cut = flow.max_flow(s, t);
    double optim_time = timer.getMsec();
    long obj_val = (long)min_cut * kr + (long)count_and * km;

    return ProblemResult(obj_val, optim_time);
}

long RelinearizeCutSolver::calcObjective(const int kr, const int km)
{
    assert(min_cut > 0);
    return (long)min_cut * kr + (long)count_and * km * 3;
}
