#include "RelinearizeGurobiSolver.hpp"

#include <sstream>

#include "gurobi_c++.h"
#include "misc.hpp"

ProblemResult RelinearizeGurobiSolver::solve(const int kr, const int km,
                                             const CircuitGraph &graph,
                                             const int max_threads)
{
    timer.set();

    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_Threads, max_threads);
    model.set(GRB_DoubleParam_MIPGap, 1e-8);

    // Allocate variables
    GRBVar *l = model.addVars(graph.n_gate, GRB_INTEGER);
    GRBVar *x = model.addVars(graph.n_gate, GRB_INTEGER);
    for (int i = 0; i < graph.n_gate; i++)
    {
        std::ostringstream lname, xname;
        lname << "l[" << i << "]";
        l[i].set(GRB_DoubleAttr_LB, 2);
        l[i].set(GRB_StringAttr_VarName, lname.str());

        xname << "x[" << i << "]";
        x[i].set(GRB_DoubleAttr_LB, 0);
        x[i].set(GRB_StringAttr_VarName, xname.str());
    }

    // Set Objective
    GRBLinExpr obj = 0;
    {
        GRBLinExpr sum_x = 0, sum_mul = 0;
        for (int i = 0; i < graph.n_gate; i++)
        {
            sum_x += x[i];
        }
        for (int i = 0; i < graph.n_gate; i++)
            if (graph.gates[i].type == AND)
            {
                sum_mul += l[i] + x[i];
            }
        obj = kr * sum_x + km * sum_mul;
    }
    model.setObjective(obj, GRB_MINIMIZE);

    // Set Constraints
    for (int i = 0; i < graph.n_gate; i++)
    {
        int p1 = graph.wires[graph.gates[i].in1].src;
        int p2 = graph.wires[graph.gates[i].in2].src;

        if (graph.gates[i].type == AND)
        {
            if (p1 != -1 && p2 != -1)
            {
                model.addConstr(l[i] == l[p1] + l[p2] - 1 - x[i]);
            }
            else if (p1 != -1 && p2 == -1)
            {
                model.addConstr(l[i] == l[p1] + 2 - 1 - x[i]);
            }
            else if (p1 == -1 && p2 != -1)
            {
                model.addConstr(l[i] == l[p2] + 2 - 1 - x[i]);
            }
            else if (p1 == -1 && p2 == -1)
            {
                model.addConstr(l[i] == 2 + 2 - 1 - x[i]);
            }
        }
        else if (graph.gates[i].type == XOR)
        {
            if (p1 != -1)
                model.addConstr(l[i] >= l[p1] - x[i]);
            if (p2 != -1)
                model.addConstr(l[i] >= l[p2] - x[i]);
        }
        else if (graph.gates[i].type == INV)
        {
            if (p1 != -1)
                model.addConstr(l[i] == l[p1] - x[i]);
        }

        // if output var, dimension must 2
        if (graph.gates[i].out >= graph.n_wire - graph.n_output)
        {
            model.addConstr(l[i] == 2);
        }
    }

    model.optimize();

    double optim_time = timer.getMsec();
    long obj_val = model.get(GRB_DoubleAttr_ObjVal);

    // TODO free variables l, x, model, and other

    long suml = 0, sumx = 0;
    for (int i = 0; i < graph.n_gate; i++)
    {
        sumx += x[i].get(GRB_DoubleAttr_X);
    }
    for (int i = 0; i < graph.n_gate; i++)
        if (graph.gates[i].type == AND)
        {
            suml += l[i].get(GRB_DoubleAttr_X) + x[i].get(GRB_DoubleAttr_X);
        }
    // we can use these variables later

    return ProblemResult(obj_val, optim_time);
}
