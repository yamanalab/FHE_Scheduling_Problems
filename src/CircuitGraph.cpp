#include "CircuitGraph.hpp"

#include <cassert>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <vector>

// TODO read blif format
CircuitGraph::CircuitGraph(const std::string &filename)
{
    std::ifstream ifs(filename);
    int n1, n2;
    ifs >> n_gate >> n_wire >> n1 >> n2 >> n_output;
    n_input = n1 + n2;
    assert(n_gate + n_input == n_wire);

    gates.resize(n_gate);
    wires.resize(n_wire + 1);

    for (int i = 0; i < n_gate; ++i)
    {
        int n_in, n_out;
        ifs >> n_in >> n_out;
        assert((n_in == 1 || n_in == 2) && n_out == 1);

        int in1, in2 = -1, out;
        std::string type;
        ifs >> in1;
        if (n_in == 2)
            ifs >> in2;
        ifs >> out >> type;

        assert(0 <= in1 && in1 <= n_wire);
        assert(0 <= out && out <= n_wire);
        if (n_in == 2)
            assert(0 <= in2 && in2 <= n_wire);

        if (type == "AND")
        {
            assert(n_in == 2 && n_out == 1);
            gates[i] = Gate(in1, in2, out, AND);
        }
        else if (type == "XOR")
        {
            assert(n_in == 2 && n_out == 1);
            gates[i] = Gate(in1, in2, out, XOR);
        }
        else if (type == "INV")
        {
            assert(n_in == 1 && n_out == 1);
            gates[i] = Gate(in1, n_wire, out, INV);
        }
        else
        { // Unknown gate type
            assert(false);
        }

        if (in1 >= 0)
            wires[in1].dests.push_back(i);
        if (in2 >= 0)
            wires[in2].dests.push_back(i);

        // every wire should have just one input (or no, for inwire)
        assert(wires[out].src == -1);
        wires[out].src = i;
    }

    for (int i = 0; i < n_input; i++)
        assert(wires[i].src == -1);
}

CircuitGraph CircuitGraph::reduceSize()
{
    std::vector<int> num_v(n_gate, -1), num_e(n_wire, -1);
    int usedIds = 0;
    std::vector<int> num2v(n_wire, -1); // from new idx to old idx

    std::queue<int> que; // queue of edge index
    for (int i = 0; i < n_input; i++)
    {
        que.push(i);
        num_e[i] = usedIds++;
    }
    for (int i = 0; i < n_gate; i++)
    {
        if (gates[i].type == AND)
        {
            que.push(gates[i].out);
            num_v[i] = usedIds++;
            num2v[num_v[i]] = i;
            num_e[gates[i].out] = num_v[i];
        }
    }

    while (!que.empty())
    {
        int i = que.front();
        que.pop(); // edge(wire) index
        for (auto &gi : wires[i].dests)
        {
            if (gates[gi].type == AND)
                continue;
            bool toNext = false;
            if (num_v[gi] == -1)
            {
                // first visit of the node
                num_v[gi] = num_e[i];
                if (gates[gi].type == INV)
                    toNext = true;
            }
            else
            {
                // second visit of the node
                if (num_v[gi] != num_e[i])
                {
                    num_v[gi] = usedIds++;
                    num2v[num_v[gi]] = gi;
                }
                toNext = true;
            }
            if (toNext)
            {
                num_e[gates[gi].out] = num_v[gi];
                que.push(gates[gi].out);
            }
        }
    }

    // edge_index -> num_v
    std::map<int, int> input_vertices;

    std::vector<Gate> ng;
    std::vector<Wire> nw;

    // input wires : the same as original
    // output wires : build below
    std::set<int> out_wires, general_wires;
    {
        // re-numbering wires and gates
        std::map<int, int> nume2new_idx, numv2new_idx;
        for (int i = n_wire - n_output; i < n_wire; i++)
        {
            out_wires.insert(num_e[i]);
        }
        for (int i = n_input; i < n_wire - n_output; i++)
        {
            if (num_e[i] >= n_input &&
                out_wires.find(num_e[i]) == out_wires.end())
                general_wires.insert(num_e[i]);
        }

        int cnt = 0;
        for (int i = 0; i < n_input; i++)
            nume2new_idx[i] = cnt++;
        for (auto i : general_wires)
            nume2new_idx[i] = cnt++;
        for (auto i : out_wires)
            nume2new_idx[i] = cnt++;
        nw.resize(cnt);

        cnt = 0;
        for (int i = 0; i < n_gate; i++)
        {
            if (numv2new_idx.find(num_v[i]) == numv2new_idx.end())
                numv2new_idx[num_v[i]] = cnt++;
        }
        ng.resize(cnt);

        for (int i = 0; i < n_gate; i++)
        {
            if ((gates[i].type == XOR && gates[i].in1 == gates[i].in2 &&
                 gates[i].in1 != -1 && num_e[gates[i].in1] < n_input) ||
                (gates[i].type == INV && num_e[gates[i].in1] < n_input))
            {
                input_vertices[num_e[gates[i].in1]] = numv2new_idx[num_v[i]];
            }
        }

        for (int i = 0; i < n_wire; i++)
            num_e[i] = nume2new_idx[num_e[i]];
        for (int i = 0; i < n_gate; i++)
            num_v[i] = numv2new_idx[num_v[i]];
    }

    // trick to treat input-edge as a vertex
    int trick_add_ng_cnt = 0;
    for (int i = 0; i < n_input; i++)
    {
        if (input_vertices.find(i) != input_vertices.end())
        {
            wires[i].src = num_v.size();
            num_v.push_back(input_vertices[i]);
        }
        else
        {
            wires[i].src = num_v.size();
            num_v.push_back(ng.size());
            ng.push_back(Gate(-1, -1, i, INV));
            trick_add_ng_cnt++;
        }
    }

    // set input to new vertex v from wire w
    auto setInput = [&](int v, int e)
    {
        if (ng[v].in1 == -1)
        {
            ng[v].in1 = e;
        }
        else
        {
            assert(ng[v].in2 == -1);
            ng[v].in2 = e;
        }
    };

    for (int i = 0; i < n_wire; i++)
    {
        assert(ng[num_v[wires[i].src]].out == -1 ||
               ng[num_v[wires[i].src]].out == num_e[i]);
        ng[num_v[wires[i].src]].out = num_e[i];
        assert(nw[num_e[i]].src == -1 ||
               nw[num_e[i]].src == num_v[wires[i].src]);
        nw[num_e[i]].src = num_v[wires[i].src];

        for (auto to : wires[i].dests)
        {
            if (num_v[wires[i].src] != num_v[to])
            {
                nw[num_e[i]].dests.push_back(num_v[to]);
                setInput(num_v[to], num_e[i]);
            }
        }
    }

    // remove a trick to treat input-edge as a vertex
    for (auto &p : input_vertices)
    {
        ng[p.second].in1 = num_e[p.first];
        ng[p.second].in2 = num_e[p.first];
    }
    for (int i = 0; i < n_input; i++)
    {
        wires[i].src = -1;
        nw[num_e[i]].src = -1;
        if (i < trick_add_ng_cnt)
            ng.pop_back();
    }

    for (int i = 0; i < (int)ng.size(); i++)
    {
        ng[i].type = (ng[i].in2 == -1 ? INV : XOR);
    }
    int countAND = 0;
    for (int i = 0; i < n_gate; i++)
    {
        if (gates[i].type == AND)
        {
            ng[num_v[i]].type = AND;
            countAND++;
        }
    }

    CircuitGraph newGraph(ng, nw, n_input, out_wires.size());

    return newGraph;
}
