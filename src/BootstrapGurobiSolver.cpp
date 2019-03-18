#include "BootstrapGurobiSolver.hpp"

#include "gurobi_c++.h"
#include "misc.hpp"

#include <sstream>

/*
  === About Treatment of Level Change ===
  We assume that initiall noise level of each FHE ciphertexts are `L` and
  this level goes down by one through multiplication.
  Levels should be positive in circuit, which means that level cannot be zero or negative.
  After bootstrapping, the level go up to `N`.

  Inside this program, levels are converted from
    'L ... 1 -(boot)-> N ... 1 -(boot)-> N ...'
    to
    '0 ... Lmax -(boot)-> (L-N) ... Lmax -(boot)-> (L-N) ...'
*/

ProblemResult BootstrapGurobiSolver::solve(const int l, const int n, const CircuitGraph &graph, const int max_threads){
  int Lmax = l - 1;
  int noise_after_bootstrap = l - n;
  timer.set();

  GRBModel model = GRBModel(env);
  model.set(GRB_IntParam_LogToConsole, 0);
  model.set(GRB_IntParam_Threads, max_threads);
  model.set(GRB_DoubleParam_MIPGap, 0);

  GRBVar *B = model.addVars(graph.n_gate, GRB_BINARY);
  GRBVar *Gin = model.addVars(graph.n_gate, GRB_INTEGER);
  GRBVar *Gout = model.addVars(graph.n_gate, GRB_INTEGER);
  for (int i = 0; i < graph.n_gate; i++) {
    std::ostringstream bracket;
    bracket << "(" << i << ")";
    B[i].set(GRB_StringAttr_VarName, "B" + bracket.str());

    Gin[i].set(GRB_DoubleAttr_UB, Lmax);
    Gin[i].set(GRB_DoubleAttr_LB, 0);
    Gin[i].set(GRB_StringAttr_VarName, "Gin" + bracket.str());

    Gout[i].set(GRB_DoubleAttr_UB, Lmax);
    Gout[i].set(GRB_DoubleAttr_LB, 0);
    Gout[i].set(GRB_StringAttr_VarName, "Gout" + bracket.str());
  }

  GRBVar *InWire = model.addVars(graph.n_input, GRB_INTEGER);
  for (int i = 0; i < graph.n_input; i++) {
    std::ostringstream vname;
    vname << "InWire(" << i << ")";
    model.addConstr(InWire[i] == 0);
    InWire[i].set(GRB_StringAttr_VarName, vname.str());
  }

  // Set Objective : minimize the number of bootstrappings
  GRBLinExpr obj = 0;
  for (int i = 0; i < graph.n_gate; i++) {
    obj += B[i];
  }
  model.setObjective(obj, GRB_MINIMIZE);

  // Set main constraints
  for (int i = 0; i < graph.n_gate; ++i) {
    auto selectInputGate = [&](int wire_idx) {
      if(wire_idx < graph.n_input) return &InWire[wire_idx]; // input wire
      else return &Gout[graph.wires[wire_idx].src]; // general
    };

    GRBVar &gin1 = *selectInputGate(graph.gates[i].in1);
    GRBVar &gin2 = *selectInputGate(graph.gates[i].in2);
    GRBVar &gin = Gin[i];
    GRBVar &gout = Gout[i];
    GRBVar &bi = B[i];

    // Constraints on bootstrapping
    model.addGenConstrIndicator(bi, true, gout == noise_after_bootstrap);
    model.addGenConstrIndicator(bi, false, gout - gin == 0);

    // Constraints (gate specific)
    if (graph.gates[i].type == AND){
      model.addConstr(gin - gin1 >= 1);
      model.addConstr(gin - gin2 >= 1);
      model.addConstr(gin1 <= (Lmax-1));
      model.addConstr(gin2 <= (Lmax-1));
    } else if(graph.gates[i].type == XOR){
      model.addConstr(gin - gin1 >= 0);
      model.addConstr(gin - gin2 >= 0);
    } else { // INV
      model.addConstr(gin - gin1 >= 0);
    }

    // PV15 paper says output should strictly less than Lmax
    if(graph.gates[i].out >= graph.n_input - graph.n_wire) {
      model.addConstr(gout <= (Lmax - 1));
    }
  }

  model.optimize();

  int optimstatus = model.get(GRB_IntAttr_Status);
  if (optimstatus == GRB_INF_OR_UNBD) {
    std::cout << "retrying optimization ... " << std::endl;
    model.set(GRB_IntParam_Presolve, 0);
    model.optimize();
    optimstatus = model.get(GRB_IntAttr_Status);
  }

  double optim_time = timer.getMsec();

  // Extract result
  if (optimstatus == GRB_OPTIMAL) {
    std::vector<int> pos;
    for (int i = 0; i < graph.n_gate; i++) {
      if (abs(B[i].get(GRB_DoubleAttr_X) - 1) <= 1e-6) {
        pos.push_back(i);
      }
    }
    // dbg(pos);

  } else if (optimstatus == GRB_INFEASIBLE) {
    std::cerr << ioscc::red << "Model is infeasible" << std::endl;
  } else if (optimstatus == GRB_UNBOUNDED) {
    std::cerr << ioscc::red << "Model is unbounded" << std::endl;
  } else {
    std::cerr << ioscc::red << "Optimization was stopped with status = " << optimstatus << std::endl;
  }

  long obj_val = model.get(GRB_DoubleAttr_ObjVal);

  // TODO free variables l, x, model, and other

  return ProblemResult(obj_val, optim_time);
}
