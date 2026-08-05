---
title: FormalPowerSeries
documentation_of: ./formal_power_series.hpp
---

## FormalPowerSeries

```C++
formal_power_series_lib::FPS<Mint>
```

`Modint.hpp` と `convolution.hpp` を使う形式的冪級数・多項式ライブラリ。
係数は昇べき順で、`f[i]` は `x^i` の係数を表す。

`formal_power_series.hpp` も **提出コードにコピペして使う前提**。
`// ===== BEGIN FORMAL POWER SERIES COPY BLOCK =====` から
`// ===== END FORMAL POWER SERIES COPY BLOCK =====` までを、
**`Modint.hpp` → `convolution.hpp` の COPY BLOCK の後ろに**この順で貼る
（各ファイルの `#ifndef` ガードが効いて重複 include を避けるため、貼る順序が重要）。

### 基本的な使い方（ローカル開発）

```C++
#include "library/cpp/polynomial/formal_power_series.hpp"

using mint = Modint9;
using fps = formal_power_series_lib::FPS<mint>;

fps f = {1, 2, 3};       // 1 + 2x + 3x^2
fps g = {4, 5};          // 4 + 5x
fps product = f * g;
fps inverse = f.inv(8);  // f^{-1} mod x^8
mint value = f.eval(10);
```

`prefix(n)`、`inv(n)`、`log(n)` など、引数 `n` を取るFPS演算は
`x^n` 以上を捨て、長さ `n` の係数列を返す。

### 基本的な使い方（提出用コピペ）

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ
// 2) convolution.hpp の BEGIN/END マーカー範囲をその後ろにコピペ
// 3) formal_power_series.hpp の BEGIN/END マーカー範囲をさらにその後ろにコピペ

int main() {
    using mint = Modint9;
    using fps = formal_power_series_lib::FPS<mint>;
    fps f = {1, 2, 3};
}
```

### 基本演算

```C++
f + g
f - g
f + scalar
f - scalar
f * g
f * scalar
f / scalar
f += g
f -= g
f *= g
f *= scalar
f /= scalar
```

```C++
fps f.prefix(int n) const
fps f.pre(int n) const
void f.shrink()
fps& f.normalize()
fps f.reversed() const
fps f.rev() const
fps f.shifted_left(int n) const
fps f.shifted_right(int n) const
f << n
f >> n
fps f.mul_trunc(const fps& g, int n) const
mint f.eval(mint x) const
int f.count_nonzero(int n = -1) const
```

- `prefix(n)`: 長さ `n` に切り詰める。不足分は0で埋める。
- `pre(n)`、`rev()`、`diff()` は参考実装に合わせた短縮名。
- `shrink()`: 末尾の0を除去する。零多項式は空になる。
- `normalize()` は `shrink()` を行い、自身への参照を返す。
- `shifted_left(n)`: `x^n f(x)`。
- `shifted_right(n)`: `x^n` で割った商に相当する係数シフト。
- `mul_trunc(g, n)`: `f g mod x^n`。

### 微分・積分

```C++
fps f.derivative() const
fps f.integral() const
```

積分定数は0。`integral()` は `1, 2, ...` の逆元を使うため、
必要な整数が法の下で可逆でなければならない。

### FPS演算

```C++
fps f.inv(int n) const
fps f.series_div(const fps& g, int n) const
fps f.log(int n) const
fps f.exp(int n) const
fps f.pow(long long k, int n) const
std::optional<fps> f.sqrt(int n) const
```

前提条件:

- `inv(n)`: `f[0] != 0`
- `series_div(g, n)`: `g[0] != 0`
- `log(n)`: `f[0] == 1`
- `exp(n)`: `f[0] == 0`
- `pow(k, n)`: `k >= 0`
- `sqrt(n)`: 法が奇素数。平方根が存在しなければ `std::nullopt`

```C++
fps f = {1, 2, 1};
auto root = f.sqrt(3);
if (root) {
    // (*root) * (*root) == f (mod x^3)
}
```

### 多項式演算

```C++
std::pair<fps, fps> f.divmod(const fps& g) const
fps f.quotient(const fps& g) const
fps f.remainder(const fps& g) const
fps f.taylor_shift(mint c) const
```

`divmod` は多項式としての商と余りを返す。
`series_div` はFPSとして先頭から係数を求める操作なので、用途が異なる。

`taylor_shift(c)` は `f(x + c)` を返す。階乗の逆元を使うため、
通常は `degree(f) < Mint::mod()` が必要。

### 合成・合成逆関数

```C++
fps f.compose(const fps& g, int n) const
fps f.compositional_inverse(int n) const
```

- `compose(g, n)`: `f(g(x)) mod x^n`
- `compositional_inverse(n)`: `f(g(x)) = x mod x^n` を満たす `g`
- 合成逆関数には `f[0] == 0` かつ `f[1] != 0` が必要。

合成はHorner法を使うため、大きな次数に対しては主要なFPS演算より重い。

### 多点評価・補間

```C++
std::vector<mint> f.multipoint_evaluate(const std::vector<mint>& xs) const

fps formal_power_series_lib::interpolate(
    const std::vector<mint>& xs,
    const std::vector<mint>& ys)
```

補間点 `xs` は相異なる必要がある。

### Bostan-Mori・線形漸化式

```C++
mint formal_power_series_lib::bostan_mori(fps p, fps q, long long n)

mint formal_power_series_lib::linear_recurrence_nth(
    const std::vector<mint>& initial,
    const std::vector<mint>& recurrence,
    long long n)

std::vector<mint> formal_power_series_lib::berlekamp_massey(
    const std::vector<mint>& sequence)
```

`bostan_mori(p, q, n)` は `[x^n] p(x)/q(x)` を返す。`q[0] != 0` が必要。

`linear_recurrence_nth` の `recurrence = {c0, ..., c(d-1)}` は

```text
a[n] = c0*a[n-1] + c1*a[n-2] + ... + c(d-1)*a[n-d]
```

を表す。`initial` には `a[0]` から `a[d-1]` を渡す。
`berlekamp_massey` の返り値も同じ並び。

### 計算量

`M(n)` を次数 `n` の畳み込み計算量とする。

- 加減算、微分、積分、評価: `O(n)`
- 乗算: `O(M(n))`
- `inv`, `log`, `exp`, `pow`, `sqrt`: `O(M(n) log n)`
- 多項式 `divmod`: `O(M(n) log n)`
- 多点評価、補間: `O(M(n) log^2 n)`
- Taylor shift: `O(M(n))`
- Bostan-Mori: `O(M(d) log n)`
- Berlekamp-Massey: `O(n^2)`
- `compose`: Horner法による `O(deg(f) M(n))`

`Modint9` では通常 `M(n) = O(n log n)`。
`inv`、`log`、`exp` は対象範囲の非零項が60個以下なら、参考実装と同様に
係数漸化式を自動選択し `O(nk)`（`k` は非零項数）で計算する。

### 注意

- 高速化と逆元の前提から、基本的には素数法の `Modint9` を推奨する。
- `Modint1` や動的modでは通常CRT畳み込みを使う。NTT長や係数上界が
  CRTの対応範囲を超えた場合のみ `O(nm)` にフォールバックする。
- `assert` で示した前提条件を満たさない入力は対象外。
- 空配列は零多項式として扱う箇所がある。

