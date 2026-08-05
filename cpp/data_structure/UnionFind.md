---
title: UnionFind
documentation_of: ./union_find.hpp
---

## UnionFind
```C++
class UnionFind
```

### 使い方
```C++
UnionFind uf(n);
uf.unite(a, b);
if (uf.same(x, y)) { /* ... */ }
```

### 関数一覧

**コンストラクタ**
```C++
UnionFind uf(int n)
```
0-indexed で n 要素の Union-Find を作成する。

**find**
```C++
int uf.find(int x)
```
要素 x の根（代表）を返す。経路圧縮を行う。

**same**
```C++
bool uf.same(int a, int b)
```
a と b が同じ集合に属するかを返す。

**unite**
```C++
bool uf.unite(int a, int b)
```
a と b を併合する。すでに同じ集合なら false を返す。サイズの小さい木を大きい木にぶら下げる。

**size**
```C++
int uf.size(int x)
```
要素 x が属する集合のサイズを返す。

**roots**
```C++
std::vector<int> uf.roots()
```
各連結成分の根（代表）を列挙して返す。

**groups**
```C++
std::vector<std::vector<int>> uf.groups()
```
連結成分ごとの要素一覧を返す。

### 計算量

- `find` / `same` / `unite` / `size`: ほぼ $ O(\alpha(N)) $
- `roots` / `groups`: 全体で $ O(N \alpha(N)) $

### Union-Find とは？

要素集合を「互いに素な集合（連結成分）」として管理するデータ構造。
以下の操作を高速に行える。

- 2 つの要素が同じ集合かどうかの判定
- 2 つの集合の併合

典型的な用途は、連結成分管理・最小全域木（Kruskal法）など。

