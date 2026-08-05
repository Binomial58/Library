---
title: Matrix
documentation_of: ./matrix.hpp
---

## Matrix
```C++
matrix_lib::Matrix<T>
matrix_lib::mat_mul(A, B, combine, aggregate, e_combine)
matrix_lib::mat_pow(A, n, combine, aggregate, e_combine, e_aggregate)
```

`combine`（畳み込み演算）と `aggregate`（各項の演算）を外から渡せる汎用行列。
`(+, ×)` を渡せば通常の行列積・行列累乗（mod付き線形漸化式、グラフの経路数など）、
`(min, +)` を渡せば min-plus 行列（ダブリングによる最短コスト計算）、
`(OR, AND)` を渡せば到達可能性の行列版など、同じ実装を使い回せる。

### 使い方

**通常の行列積・行列累乗（mod付き）**
```C++
#include "library/cpp/math/matrix.hpp"
using ll = long long;
const ll MOD = 998244353;

ll add(ll a, ll b) { return (a + b) % MOD; }
ll mul(ll a, ll b) { return a * b % MOD; }

matrix_lib::Matrix<ll> A(n, n), B(n, n);
// ... 値を詰める ...

auto C = matrix_lib::mat_mul(A, B, add, mul, /*e_combine=*/0LL);
auto P = matrix_lib::mat_pow(A, k, add, mul, /*e_combine=*/0LL, /*e_aggregate=*/1LL); // A^k
```

**min-plus 行列（ダブリングによる最短コスト計算, ABC445F 相当）**
```C++
#include "library/cpp/math/matrix.hpp"
using ll = long long;
const ll INFLL = (1LL << 62);

ll rmin(ll a, ll b) { return std::min(a, b); }
auto add = [](ll a, ll b) { return matrix_lib::capped_add(a, b, INFLL); }; // INF+INFの桁あふれを防ぐ

matrix_lib::Matrix<ll> C(n, n);
// ... C[i][j] = 辺 i->j のコスト、無ければ INFLL ...

auto P = matrix_lib::mat_pow(C, k, rmin, add, /*e_combine=*/INFLL, /*e_aggregate=*/0LL); // k 回後の最短コスト
```

### 関数一覧

**コンストラクタ**
```C++
matrix_lib::Matrix<T> M(int h, int w, const T& fill = T())
matrix_lib::Matrix<T> M(std::vector<std::vector<T>> data)
```
`h x w` を `fill` で初期化、または二次元配列から構築する。

**operator[]**
```C++
std::vector<T>& M[int i]
```
`i` 行目の参照を返す（`M[i][j]` で要素アクセス）。

**height / width**
```C++
int M.height()
int M.width()
```

**identity_like**
```C++
matrix_lib::Matrix<T> matrix_lib::identity_like(int n, const T& e_combine, const T& e_aggregate)
```
対角成分が `e_aggregate`、それ以外が `e_combine` の `n x n` 行列を作る
（`combine`/`aggregate` に対する単位行列。`mat_pow` の初期値として使われる）。

**mat_mul**
```C++
matrix_lib::Matrix<T> matrix_lib::mat_mul(const Matrix<T>& A, const Matrix<T>& B,
                                           Combine combine, Aggregate aggregate,
                                           const T& e_combine)
```
`C[i][j] = combine_{k}( aggregate(A[i][k], B[k][j]) )` を計算する。
`A` の列数と `B` の行数は一致している必要がある。

**mat_pow**
```C++
matrix_lib::Matrix<T> matrix_lib::mat_pow(Matrix<T> A, long long n,
                                           Combine combine, Aggregate aggregate,
                                           const T& e_combine, const T& e_aggregate)
```
`A`（正方行列）を `n` 乗した行列を二分累乗で返す。

**capped_add**
```C++
T matrix_lib::capped_add(const T& a, const T& b, const T& cap)
```
`a` か `b` が `cap` 以上なら `cap` を、そうでなければ `a + b` を返す。
min-plus 行列で aggregate に `+` を渡すとき、「到達不可(+INF)」同士の加算が
桁あふれするのを防ぐために使う（下記「注意」参照）。

### `combine` / `aggregate` と単位元の対応

| 用途 | combine | aggregate | e_combine | e_aggregate |
| --- | --- | --- | --- | --- |
| 通常の行列積（mod付き） | `+` | `×` | `0` | `1` |
| min-plus（最短コスト） | `min` | `+` | `+INF` | `0` |
| max-plus | `max` | `+` | `-INF` | `0` |
| 到達可能性 | `OR` | `AND` | `false` | `true` |

- `e_combine` は `mat_mul` の結果行列の初期値（畳み込みの単位元）に使う。
- `e_aggregate` は `mat_pow` の単位行列の対角成分に使う。

### 計算量

- `mat_mul`: `O(h * mid * w)`（`h x mid` と `mid x w` の積）
- `mat_pow`: `O(n^3 log e)`（`n x n` 行列を `e` 乗する場合）

### 前提条件

- `combine` は結合則・交換則を満たすこと（`e_combine` を単位元とするモノイド）。
- `aggregate` は `combine` に対して分配則を満たすこと
  （`aggregate(a, combine(b, c)) == combine(aggregate(a, b), aggregate(a, c))`、他の引数位置も同様）。
- `mat_pow` を使う場合はさらに `aggregate(e_aggregate, x) == aggregate(x, e_aggregate) == x` を満たすこと。

### 注意: INF同士のオーバーフロー

min-plus（`combine=min`, `aggregate=+`）で `e_combine` に `INFLL` のような大きな番兵値を使う場合、
「到達不可」同士を素の `+` で畳み込むと `INF + INF` が `long long` の範囲を超えて桁あふれし、
無関係に小さい（時に負の）値へ化けることがある。`aggregate` にはそのまま `+` を渡さず、
`capped_add` で包んで `INFLL + INFLL` が発生しても `INFLL` のままになるようにすること。
