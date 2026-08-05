#pragma once

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace graph_lib
{
    namespace internal
    {
        template <class E>
        struct csr
        {
            std::vector<int> start;
            std::vector<E> elist;

            explicit csr(int n, const std::vector<std::pair<int, E>> &edges)
                : start(n + 1), elist(edges.size())
            {
                for (const auto &e : edges)
                {
                    ++start[e.first + 1];
                }
                for (int i = 1; i <= n; ++i)
                {
                    start[i] += start[i - 1];
                }
                auto counter = start;
                for (const auto &e : edges)
                {
                    elist[counter[e.first]++] = e.second;
                }
            }
        };
    } // namespace internal

    // 有向グラフの強連結成分分解
    class scc_graph
    {
    public:
        explicit scc_graph(int n) : n_(n)
        {
            assert(0 <= n);
        }

        int num_vertices() const
        {
            return n_;
        }

        void add_edge(int from, int to)
        {
            assert(0 <= from && from < n_);
            assert(0 <= to && to < n_);
            edges_.push_back({from, edge{to}});
        }

        // 戻り値: (強連結成分数, 各頂点の成分番号)
        std::pair<int, std::vector<int>> scc_ids() const
        {
            auto g = internal::csr<edge>(n_, edges_);
            int now_ord = 0, group_num = 0;
            std::vector<int> visited, low(n_), ord(n_, -1), ids(n_);
            visited.reserve(n_);

            auto dfs = [&](auto self, int v) -> void
            {
                low[v] = ord[v] = now_ord++;
                visited.push_back(v);
                for (int i = g.start[v]; i < g.start[v + 1]; ++i)
                {
                    const int to = g.elist[i].to;
                    if (ord[to] == -1)
                    {
                        self(self, to);
                        low[v] = std::min(low[v], low[to]);
                    }
                    else
                    {
                        low[v] = std::min(low[v], ord[to]);
                    }
                }
                if (low[v] == ord[v])
                {
                    while (true)
                    {
                        const int u = visited.back();
                        visited.pop_back();
                        ord[u] = n_;
                        ids[u] = group_num;
                        if (u == v)
                            break;
                    }
                    ++group_num;
                }
            };

            for (int i = 0; i < n_; ++i)
            {
                if (ord[i] == -1)
                {
                    dfs(dfs, i);
                }
            }
            for (int &id : ids)
            {
                id = group_num - 1 - id;
            }
            return {group_num, ids};
        }

        std::vector<std::vector<int>> scc() const
        {
            auto [group_num, ids] = scc_ids();
            std::vector<int> counts(group_num);
            for (const int id : ids)
            {
                ++counts[id];
            }

            std::vector<std::vector<int>> groups(group_num);
            for (int i = 0; i < group_num; ++i)
            {
                groups[i].reserve(counts[i]);
            }
            for (int v = 0; v < n_; ++v)
            {
                groups[ids[v]].push_back(v);
            }
            return groups;
        }

    private:
        struct edge
        {
            int to;
        };

        int n_;
        std::vector<std::pair<int, edge>> edges_;
    };
} // namespace graph_lib
