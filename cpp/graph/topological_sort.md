---
title: TopologicalSort
documentation_of: ./topological_sort.hpp
---

## Topological Sort
```C++
graph_lib::topological_sort(graph)
```

有向グラフをトポロジカルソートするライブラリ。
入次数が 0 の頂点をキューで処理する Kahn のアルゴリズム。

### 対応するグラフ

次の形のグラフを想定する。

- `graph.n` で頂点数を取得できる
- `graph[v]` で頂点 `v` から出る辺を走査できる
- 辺は `int`、または `edge.to` で行き先を取得できる型

現在のテンプレートにある `Graph` と `WeightedGraph` の両方で使える。

### 使い方
```C++
#include "library/cpp/graph/topological_sort.hpp"

Graph graph(4);
graph.add_edge(0, 1, false);
graph.add_edge(0, 2, false);
graph.add_edge(1, 3, false);
graph.add_edge(2, 3, false);

std::vector<int> order = graph_lib::topological_sort(graph);
bool is_dag = (int)order.size() == graph.n;
```

### 関数一覧

**topological_sort**
```C++
template <class Graph>
std::vector<int> graph_lib::topological_sort(const Graph& graph)
```

`graph` をトポロジカルソートした頂点列を返す。
閉路がある場合、閉路に含まれる頂点とその後続頂点は返らないことがある。
DAG かどうかは、返り値の長さが `graph.n` と等しいかで判定できる。

### 計算量

- `O(n + m)`

ここで `n` は頂点数、`m` は辺数。

### 注意

- 頂点番号は `0` から `n - 1` を使う。
- 無向辺を追加すると基本的に閉路になるため、`Graph::add_edge(u, v, false)` のように有向辺として追加する。
