---
title: Lazy SegTree
documentation_of: ./lazy_segtree.hpp
---

## Lazy SegTree

```cpp
lazy_segtree_lib::lazy_segtree<S, op, e, F, mapping, composition, id>
```

区間更新と区間クエリを `O(log n)` で処理する遅延セグメント木。

これが必要になるのは、例えば次のようなとき。

- 区間 `[l, r)` に `x` を加算したい
- 区間 `[l, r)` を `x` に代入したい
- そのたびに区間和 / 区間最小値 / 区間最大値を取りたい

普通の Segtree と違って、`更新そのもの` をノードにためて後から押し下げる。

## まず何を決めるか

このライブラリは、次の 2 つを自分で設計してから使う。

- `S`: 各区間に持たせる情報
- `F`: 区間にあとで適用する更新

例:

- 区間最大値が欲しいだけなら `S = long long`
- 区間和が欲しいなら `S = {sum, size}`
- 区間加算なら `F = 加算量`
- 区間代入なら `F = 代入値 or 何もしない`

## 5個の関数の意味

```cpp
S op(S a, S b);            // 2 区間をマージ
S e();                     // op の単位元
S mapping(F f, S x);       // 区間情報 x に更新 f を適用
F composition(F f, F g);   // 先に g, 後に f を適用した合成
F id();                    // 更新が何もしない状態
```

読むときの対応はこう。

- `op`: 左右の子を親にまとめる
- `mapping`: この区間に更新を当てると値がどう変わるか
- `composition`: 更新を2回ためたら、結局どういう更新になるか

特に `composition(f, g)` の順番は重要で、`先に g, 後に f`。

## 最小例: 区間加算 + 区間和

いちばん形が分かりやすい例。

```cpp
#include "library/cpp/data_structure/lazy_segtree.hpp"

struct S
{
    long long sum;
    int size;
};

using F = long long; // 区間に足す値

S op(S a, S b) { return {a.sum + b.sum, a.size + b.size}; }
S e() { return {0LL, 0}; }

S mapping(F f, S x)
{
    return {x.sum + f * x.size, x.size};
}

F composition(F f, F g)
{
    return f + g;
}

F id() { return 0LL; }
```

意味:

- `S.sum`: その区間の和
- `S.size`: その区間の長さ
- `mapping`: 区間長が `size` なので、`f` を足すと和は `f * size` 増える
- `composition`: `+3` のあと `+5` は、まとめると `+8`

使い方:

```cpp
std::vector<S> a = {{1, 1}, {4, 1}, {2, 1}, {8, 1}, {5, 1}};
lazy_segtree_lib::lazy_segtree<S, op, e, F, mapping, composition, id> seg(a);

seg.apply(1, 4, 3);             // [1, 4) に 3 を加算
auto all = seg.prod(0, 5).sum;  // 29
auto one = seg.get(2).sum;      // 5
```

## 代入更新の考え方

区間加算より区間代入の方が詰まりやすい。

例えば「区間を `x` に代入」なら、古い代入は新しい代入で上書きされる。  
なので `F` は「値」だけでなく「何もしないかどうか」も持たせると扱いやすい。

```cpp
using S = long long;

struct F
{
    bool has_value;
    long long value;
};

S op(S a, S b) { return std::max(a, b); }
S e() { return -(1LL << 60); }

S mapping(F f, S x)
{
    if (!f.has_value) return x;
    return f.value;
}

F composition(F f, F g)
{
    // 先に g, 後に f
    if (f.has_value) return f;
    return g;
}

F id() { return {false, 0}; }
```

ここでのポイント:

- `id()` は「更新しない」
- `mapping` は代入値があれば無条件で置き換える
- `composition` は新しい代入が勝つ

## 使い方

### 構築

```cpp
lazy_segtree_lib::lazy_segtree<S, op, e, F, mapping, composition, id> seg(int n);
lazy_segtree_lib::lazy_segtree<S, op, e, F, mapping, composition, id> seg(const std::vector<S>& v);
```

- `seg(n)`: 長さ `n`、全要素 `e()` で構築
- `seg(v)`: 配列 `v` から構築

### 一点取得・一点更新

```cpp
seg.set(p, x); // a[p] = x
seg.get(p);    // a[p]
```

### 区間取得

```cpp
seg.prod(l, r); // [l, r)
seg.all_prod(); // 全体
```

このライブラリの区間は常に半開区間 `[l, r)`。

### 更新適用

```cpp
seg.apply(p, f);      // 1 点に更新
seg.apply(l, r, f);   // [l, r) に更新
```

## `max_right` / `min_left`

境界を二分探索したいときの関数。

```cpp
int r = seg.max_right(l, g);
int l = seg.min_left(r, g);
```

意味:

- `max_right(l, g)`: `g(prod(l, r)) == true` を満たす最大の `r`
- `min_left(r, g)`: `g(prod(l, r)) == true` を満たす最小の `l`

条件:

- `g(e()) == true`
- `g` は単調であることを期待する

## よくある設計パターン

よく使う定型は次を参照。

- [Lazy Segtree Cheatsheet](../../../docs/contest/lazy-segtree-cheatsheet.md)

載せているもの:

- 区間加算 + 区間最小値
- 区間加算 + 区間最大値
- 区間加算 + 区間和
- 区間代入 + 区間最小値
- 区間代入 + 区間最大値
- 区間代入 + 区間和

## 計算量

- 構築: `O(n)`
- `set` / `get` / `prod` / `apply` / `max_right` / `min_left`: `O(log n)`
- `all_prod`: `O(1)`

## 満たすべき条件

少なくとも次が必要。

- `op(op(a, b), c) == op(a, op(b, c))`
- `op(a, e()) == op(e(), a) == a`
- `mapping(id(), x) == x`
- `mapping(f, mapping(g, x)) == mapping(composition(f, g), x)`
- `mapping(f, op(x, y)) == op(mapping(f, x), mapping(f, y))`

## よくあるハマりどころ

- `composition(f, g)` の順番を逆に書く
- 区間和なのに `size` を持たない
- `e()` を 0 にしてしまい、最小値/最大値で壊す
- 区間を閉区間 `[l, r]` のつもりで使ってしまう
- 代入更新なのに `id()` と通常値の区別がつかない設計にする
