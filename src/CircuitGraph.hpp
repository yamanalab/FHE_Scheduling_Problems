#ifndef _FHE_CIRCUIT_GRAPH
#define _FHE_CIRCUIT_GRAPH

#include <string>
#include <vector>

// currently, we only think these three type of gates
enum gate_type
{
    XOR,
    AND,
    INV,
    IN,
    OUT
};

// gates and wires are 0-indexed
class Gate
{
public:
    int in1 = -1, in2 = -1; // input wires of this gae
    int out = -1;           // output wire of this gate
    gate_type type;
    Gate(int _in1, int _in2, int _out, gate_type _type)
      : in1(_in1), in2(_in2), out(_out), type(_type){};
    Gate(){};
};

class Wire
{
public:
    int src = -1; // source gate of this wire
    Wire(){};
    std::vector<int> dests;
};

class CircuitGraph
{
private:
    CircuitGraph();
    CircuitGraph(const std::vector<Gate> &ng, const std::vector<Wire> &nw,
                 const int nin, const int nout)
      : n_gate(ng.size()),
        n_wire(nw.size()),
        n_input(nin),
        n_output(nout),
        gates(ng),
        wires(nw){};

public:
    int n_gate;
    int n_wire;
    int n_input;
    int n_output;

    std::vector<Gate> gates;
    std::vector<Wire> wires;

    CircuitGraph(const std::string &filename);

    CircuitGraph reduceSize();
};

#endif // _FHE_CIRCUIT_GRAPH
