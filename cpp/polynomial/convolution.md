---
title: Convolution
documentation_of: ./convolution.hpp
---

## Convolution

```C++
convolution_lib::convolution(a, b)
```

`Modint.hpp` の modint を係数に持つ多項式の畳み込みを計算する。
係数は昇べき順で、`a[i]` は `x^i` の係数を表す。

`convolution.hpp` も **提出コードにコピペして使う前提**。
`// ===== BEGIN CONVOLUTION COPY BLOCK =====` から
`// ===== END CONVOLUTION COPY BLOCK =====` までを、
**`Modint.hpp` の COPY BLOCK の後ろに**貼る
（`Modint.hpp` 側の `#ifndef` ガードが効いて重複 include を避けるため、貼る順序が重要）。

### 使い方（ローカル開発）

```C++
#include "library/cpp/polynomial/convolution.hpp"

using mint = Modint9;

std::vector<mint> a = {1, 2, 3};
std::vector<mint> b = {4, 5};
auto c = convolution_lib::convolution(a, b);
// c = {4, 13, 22, 15}
```

### 使い方（提出用コピペ）

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ
// 2) convolution.hpp の BEGIN/END マーカー範囲をその後ろにコピペ

int main() {
    using mint = Modint9;
    std::vector<mint> a = {1, 2, 3};
    std::vector<mint> b = {4, 5};
    auto c = convolution_lib::convolution(a, b);
}
```

### 関数一覧

```C++
template <class Mint>
std::vector<Mint> convolution(const std::vector<Mint>& a,
                              const std::vector<Mint>& b)
```

- どちらかが空なら空配列を返す。
- `Mint` は `Modint.hpp` の `Modint9`、`Modint1`、`Modint`、
  `StaticModint<M>`、`DynamicModint<id>` のいずれかを想定する。
- 小さい入力は愚直法を使う。
- 法が素数で、必要な2冪乗根が存在する場合はNTTを使う。
- 直接NTTできない法では、係数を一意に復元できる範囲なら3つのNTT素数とCRTを使う。
- NTT長や係数上界がCRTの対応範囲を超える場合は、正しさを保つため愚直法へフォールバックする。

```C++
template <class Mint>
std::vector<Mint> convolution_naive(const std::vector<Mint>& a,
                                    const std::vector<Mint>& b)
```

常に愚直法で計算する。検証や小さい入力向け。

### 計算量

- NTTまたはCRTが使える場合: `O((n + m) log(n + m))`
- フォールバック時: `O(nm)`

`Modint9` の法 `998244353` では、NTT長が `2^23` 以下なら直接NTTを利用できる。

