---
title: FenwickTree
documentation_of: ./fenwick_tree.hpp
---

## FenwickTree
```C++
fenwick_tree_lib::fenwick_tree<T>
```

1 点加算と区間和を扱う Fenwick Tree（Binary Indexed Tree）。
添字は 0-indexed、区間は半開区間 `[l, r)`。

### 使い方
```C++
#include "library/cpp/data_structure/fenwick_tree.hpp"

std::vector<long long> a = {1, 4, 2, 8, 5};
fenwick_tree_lib::fenwick_tree<long long> fw(a);

fw.add(2, 10);          // a[2] += 10
long long x = fw.sum(1, 4); // a[1] + a[2] + a[3]
long long y = fw.sum(4);    // a[0] + ... + a[3]
```

### 関数一覧

**コンストラクタ**
```C++
fenwick_tree_lib::fenwick_tree<T> fw(int n)
fenwick_tree_lib::fenwick_tree<T> fw(const std::vector<T>& v)
```
`n` 個の 0、または配列 `v` から構築する。

**add**
```C++
void fw.add(int p, T x)
```
`a[p] += x` を行う。

**sum**
```C++
T fw.sum(int r)
T fw.sum(int l, int r)
```
`sum(r)` は `[0, r)` の和、`sum(l, r)` は `[l, r)` の和を返す。

**size**
```C++
int fw.size()
```
要素数を返す。

### 計算量

- 構築: `O(n)`
- `add` / `sum`: `O(log n)`
- `size`: `O(1)`

### 注意

- `T` は加算・減算・0 初期化ができる型を想定。
- 整数型では AtCoder Library と同様、内部で unsigned 型を使ってオーバーフローによる未定義動作を避ける。

