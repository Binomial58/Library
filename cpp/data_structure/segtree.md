---
title: SegTree
documentation_of: ./segtree.hpp
---

## SegTree
```C++
segtree_lib::segtree<S, op, e>
```

区間をモノイドでまとめるセグメント木。

### 使い方
```C++
#include "library/cpp/data_structure/segtree.hpp"
using ll = long long;

ll op(ll a, ll b) { return std::max(a, b); }
ll e() { return 0LL; }

std::vector<ll> a = {1, 4, 2, 8, 5};
segtree_lib::segtree<ll, op, e> seg(a);

auto x = seg.prod(1, 4); // max(4,2,8) = 8
seg.set(2, 10);
auto y = seg.all_prod(); // 全体 max
```

### 関数一覧

**コンストラクタ**
```C++
segtree_lib::segtree<S, op, e> seg(int n)
segtree_lib::segtree<S, op, e> seg(const std::vector<S>& v)
```
`n` 個の単位元、または配列 `v` から構築する。

### `op` と `e` の定義

```C++
S op(S a, S b); // 結合則を満たす二項演算
S e();          // op の単位元
```

**set**
```C++
void seg.set(int p, S x)
```
`a[p] = x` に更新する。

**get**
```C++
S seg.get(int p)
```
`a[p]` を返す。

**prod**
```C++
S seg.prod(int l, int r)
```
半開区間 `[l, r)` の積を返す。

**all_prod**
```C++
S seg.all_prod()
```
配列全体の積を返す。

**max_right**
```C++
int seg.max_right(int l, F f)
```
`f(prod(l, r)) == true` を満たす最大の `r` を返す。

**min_left**
```C++
int seg.min_left(int r, F f)
```
`f(prod(l, r)) == true` を満たす最小の `l` を返す。

### 計算量

- 構築: `O(n)`
- `set` / `prod` / `max_right` / `min_left`: `O(log n)`
- `get` / `all_prod`: `O(1)`

### モノイド条件

- `op(op(a, b), c) == op(a, op(b, c))`
- `op(a, e()) == op(e(), a) == a`
