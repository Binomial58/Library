---
title: SCC
documentation_of: ./scc.hpp
---

## SCC

```C++
graph_lib::scc_graph
```

有向グラフを強連結成分分解するライブラリ。
AtCoder Library の `internal_scc` をベースに、`library/cpp` で単体利用しやすい形に整理してある。

### 使い方

```C++
#include "library/cpp/graph/scc.hpp"

graph_lib::scc_graph graph(5);
graph.add_edge(0, 1);
graph.add_edge(1, 2);
graph.add_edge(2, 0);
graph.add_edge(2, 3);
graph.add_edge(3, 4);

auto groups = graph.scc();
auto [group_num, ids] = graph.scc_ids();
```

### 関数一覧

**コンストラクタ**

```C++
graph_lib::scc_graph graph(int n)
```

`n` 頂点 0 辺の有向グラフを作る。

**num_vertices**

```C++
int graph.num_vertices()
```

頂点数を返す。

**add_edge**

```C++
void graph.add_edge(int from, int to)
```

有向辺 `from -> to` を追加する。

**scc_ids**

```C++
std::pair<int, std::vector<int>> graph.scc_ids()
```

強連結成分分解を行い、`(成分数, 各頂点が属する成分番号)` を返す。
成分番号は DAG 上のトポロジカル順になるように振られる。

**scc**

```C++
std::vector<std::vector<int>> graph.scc()
```

各強連結成分ごとの頂点集合を返す。
返される成分列はトポロジカル順で、各成分内の頂点順は未定義。

### 計算量

- `add_edge`: ならし `O(1)`
- `scc_ids`: `O(n + m)`
- `scc`: `O(n + m)`

ここで `n` は頂点数、`m` は辺数。

### 注意

- 頂点番号は `0` から `n - 1` を使う。
- `scc()` と `scc_ids()` は内部で全辺を走査するので、複数回呼ぶとそのたびに `O(n + m)` かかる。
