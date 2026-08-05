---
title: SetOperations
documentation_of: ./set_operations.hpp
---

## SetOperations

`std::set` で Python 風の集合演算を使うための補助ライブラリ。
`multiset` は対象外。

### 使い方
```C++
#include "library/cpp/data_structure/set_operations.hpp"

using namespace set_operations_lib;

std::set<int> a = {1, 2, 3};
std::set<int> b = {3, 4, 5};

auto uni = a | b;   // {1, 2, 3, 4, 5}
auto inter = a & b; // {3}
auto diff = a - b;  // {1, 2}
auto sym = a ^ b;   // {1, 2, 4, 5}

bool ok = is_subset(inter, uni);
```

### 演算子

```C++
a | b
a & b
a - b
a ^ b
```

- `a | b`: 和集合
- `a & b`: 積集合
- `a - b`: 差集合
- `a ^ b`: 対称差

左辺の `std::set` と同じ比較関数・アロケータの `std::set` を返す。

### 破壊的更新

```C++
a |= b
a &= b
a -= b
a ^= b
```

`a` を各集合演算の結果で置き換える。

### 判定関数

```C++
bool is_subset(a, b)
bool is_superset(a, b)
bool is_disjoint(a, b)
```

- `is_subset(a, b)`: `a` が `b` の部分集合なら `true`
- `is_superset(a, b)`: `a` が `b` の上位集合なら `true`
- `is_disjoint(a, b)`: `a` と `b` が互いに素なら `true`

### 計算量

- `a | b`: `O((|a| + |b|) log (|a| + |b|))`
- `a & b` / `a - b` / `is_subset` / `is_disjoint`: `O(|a| log |b|)`
- `a ^ b`: `O(|a| log |b| + |b| log |a| + k log k)`
