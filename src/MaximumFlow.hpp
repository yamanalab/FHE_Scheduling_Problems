#ifndef _FHE_MAX_FLOW
#define _FHE_MAX_FLOW

#include <algorithm>
#include <queue>
#include <vector>

const int INF = 1 << 30;

template <typename T>
class MaximumFlow
{
private:
    struct edge
    {
        int to;
        T cap;
        int rev;
    };
    std::vector<std::vector<edge>> Graph;
    std::vector<int> level, iter; // sからの距離,どこまで調べたか

    void bfs(int s)
    {
        std::fill(std::begin(level), std::end(level), -1);
        std::queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty())
        {
            int v = q.front();
            q.pop();
            for (auto e : Graph[v])
            {
                if (e.cap > 0 && level[e.to] < 0)
                {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }

    T dfs(int v, int t, T f)
    {
        if (v == t)
            return f;
        for (int &i = iter[v]; i < (int)Graph[v].size(); i++)
        {
            auto &e = Graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to])
            {
                T d = dfs(e.to, t, std::min(f, e.cap));
                if (d > 0)
                {
                    e.cap -= d;
                    Graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

public:
    MaximumFlow(int n)
    {
        Graph = std::vector<std::vector<edge>>(n);
        level = std::vector<int>(n);
        iter = std::vector<int>(n);
    }

    T max_flow(int s, int t)
    {
        T flow = 0;
        while (true)
        {
            bfs(s);
            if (level[t] < 0)
                break;
            std::fill(std::begin(iter), std::end(iter), 0);
            T f;
            while ((f = dfs(s, t, INF)) > 0)
            {
                flow += f;
            }
        }
        return flow;
    }

    void add_edge(int from, int to, T cap)
    {
        int tos = Graph[to].size(), froms = Graph[from].size();
        Graph[from].push_back(((edge){to, cap, tos}));
        Graph[to].push_back(((edge){from, 0, froms}));
    }
};

#endif // _FHE_MAX_FLOW
