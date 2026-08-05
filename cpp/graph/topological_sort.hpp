#pragma once

#include <cassert>
#include <queue>
#include <type_traits>
#include <vector>

namespace graph_lib
{
    namespace internal
    {
        template <class T>
        std::enable_if_t<std::is_integral<T>::value, int> to_vertex(T to)
        {
            return static_cast<int>(to);
        }

        template <class Edge>
        auto to_vertex(const Edge &edge) -> decltype(static_cast<int>(edge.to))
        {
            return static_cast<int>(edge.to);
        }
    } // namespace internal

    // 有向グラフをトポロジカルソートする
    template <class Graph>
    std::vector<int> topological_sort(const Graph &graph)
    {
        const int n = graph.n;
        std::vector<int> indegree(n);
        for (int v = 0; v < n; ++v)
        {
            for (const auto &edge : graph[v])
            {
                const int to = internal::to_vertex(edge);
                assert(0 <= to && to < n);
                ++indegree[to];
            }
        }

        std::queue<int> que;
        for (int v = 0; v < n; ++v)
        {
            if (indegree[v] == 0)
            {
                que.push(v);
            }
        }

        std::vector<int> order;
        order.reserve(n);
        while (!que.empty())
        {
            const int v = que.front();
            que.pop();
            order.push_back(v);

            for (const auto &edge : graph[v])
            {
                const int to = internal::to_vertex(edge);
                if (--indegree[to] == 0)
                {
                    que.push(to);
                }
            }
        }

        return order;
    }
} // namespace graph_lib
