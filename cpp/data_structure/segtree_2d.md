---
title: SegTree 2D
documentation_of: ./segtree_2d.hpp
---

## SegTree 2D

```cpp
segtree_2d_lib::segtree_2d<T, F>
```

二次元配列上で一点更新と矩形クエリを扱う二次元セグメント木。
矩形は半開区間 `[h1, h2) x [w1, w2)` で指定する。

可換モノイド `(T, op, e)` を渡して使う。`op` は結合則と可換性を満たす二項演算、`e` はその単位元。

## 使い方

```cpp
#include "library/cpp/data_structure/segtree_2d.hpp"

long long op(long long a, long long b)
{
    return a + b;
}

std::vector<std::vector<long long>> a = {
    {1, 2, 3},
    {4, 5, 6},
};

segtree_2d_lib::segtree_2d<long long, decltype(op) *> seg(a, op, 0LL);

long long x = seg.query(0, 1, 2, 3); // 2 + 3 + 5 + 6 = 16
seg.update(1, 2, 10);                // a[1][2] = 10
long long y = seg.get(1, 2);          // 10
```

ラムダを使う場合:

```cpp
auto mx = [](long long a, long long b) { return std::max(a, b); };
segtree_2d_lib::segtree_2d<long long, decltype(mx)> seg(h, w, mx, -(1LL << 60));
```

## 構築方法

二次元配列から直接構築できる。

```cpp
segtree_2d_lib::segtree_2d<T, F> seg(v, op, e);
```

またはサイズだけ作り、`set` で初期値を入れてから `build` する。

```cpp
segtree_2d_lib::segtree_2d<T, F> seg(h, w, op, e);
seg.set(i, j, x); // build 前だけ使う
seg.build();
```

`set` は構築用。構築後の変更には `update` を使う。

## 関数一覧

**コンストラクタ**

```cpp
segtree_2d_lib::segtree_2d<T, F> seg(int h, int w, F op, const T& e)
segtree_2d_lib::segtree_2d<T, F> seg(const std::vector<std::vector<T>>& v, F op, const T& e)
```

`h x w` の単位元配列、または二次元配列 `v` から構築する。

**set**

```cpp
void seg.set(int h, int w, const T& x)
```

`build` 前に `a[h][w] = x` を入れる。

**build**

```cpp
void seg.build()
```

`set` で入れた初期値から木を構築する。

**get**

```cpp
T seg.get(int h, int w)
T seg(int h, int w)
```

`a[h][w]` を返す。

**update**

```cpp
void seg.update(int h, int w, const T& x)
```

`a[h][w] = x` に更新する。

**query**

```cpp
T seg.query(int h1, int w1, int h2, int w2)
T seg.prod(int h1, int w1, int h2, int w2)
```

半開矩形 `[h1, h2) x [w1, w2)` の積を返す。空矩形なら単位元 `e` を返す。
`prod` は `query` の別名。

**height / width**

```cpp
int seg.height()
int seg.width()
```

元の二次元配列の縦横サイズを返す。

## 計算量

- 構築: `O(HW)`
- `update` / `query`: `O(log H log W)`
- `set` / `get` / `height` / `width`: `O(1)`
- 空間: `O(HW)`

## 注意

- 添字は 0-indexed。
- `op` は結合則と可換性を満たす必要がある。典型例は和、最小値、最大値、gcd。
- `e` は `op(a, e) == op(e, a) == a` を満たす必要がある。
- 非可換な `op` では、任意矩形の「読み順の積」としては扱わない方が安全。
- `H` または `W` が 0 の場合、`query` できるのは空矩形だけ。

## 出典

この実装は [Nyaan's Library の二次元セグメント木](https://nyaannyaan.github.io/library/data-structure-2d/2d-segment-tree.hpp) を参考に、このリポジトリ向けに `std::vector` の include、名前空間、ドキュメント、境界チェック、二次元配列からの構築を整備したもの。

元リポジトリ [NyaanNyaan/library](https://github.com/NyaanNyaan/library) は CC0-1.0 ライセンス。
