---
title: Modint
documentation_of: ./Modint.hpp
---

## Modint
```C++
Modint / Modint9 / Modint1
```

`Modint.hpp` は **提出コードにコピペして使う前提**。  
`// ===== BEGIN MODINT COPY BLOCK =====` から  
`// ===== END MODINT COPY BLOCK =====` までをそのまま貼る。

### 名前空間

- 内部実装は `namespace modint_lib`
- `modint_lib` 内でも `Modint9` / `Modint1` / `Modint` を直接定義
- 通常利用はグローバル公開名 `Modint*`（`Modint`, `Modint9`, `Modint1`）

### 使い方（固定 mod）

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ

int main() {
    Modint9 a = 10;
    Modint9 b = 3;
    Modint9 c = a / b; // 10 * 3^{-1}
    int v = c.val();
}
```

### 使い方（動的 mod）

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ

int main() {
    Modint::set_mod(998244353); // 最初に1回だけ設定
    Modint x = 123456789;
    Modint y = 2;
    Modint z = x * y;
    int v = z.val();
}
```

### 主な機能

- `+`, `-`, `*`, `/` と複合代入
- `pow(long long n)`
- `inv()`
- `val()`
- `StaticModint<M>` / `DynamicModint<id>`
- `Binomial<Mint>`（階乗・逆階乗の前計算）

### 関数一覧（共通）

`Modint9` / `Modint1` / `StaticModint<M>` / `DynamicModint<id>` で共通。

- `ModintType()`  
  `0` で初期化する。
- `ModintType(T v)`  
  `v` を `mod` で正規化して保持する（負数も可）。
- `static ModintType raw(int v)`  
  正規化せずに内部値として `v` を入れる（`0 <= v < mod` を満たすときのみ使う）。
- `int val() const`  
  現在の値（`0..mod-1`）を返す。
- `ModintType pow(long long n) const`  
  二分累乗で `n` 乗を返す（`n >= 0`）。
- `ModintType inv() const`  
  逆元を返す（存在条件を満たす必要あり）。
- `operator++` / `operator--`（前置・後置）  
  `+1` / `-1` を mod 上で行う。
- `operator+=`, `operator-=`, `operator*=`, `operator/=`  
  複合代入。
- `operator+`, `operator-`, `operator*`, `operator/`  
  四則演算。
- `operator==`, `operator!=`  
  等値比較。

### 関数一覧（固定 mod 側）

- `static constexpr int mod()`  
  法を返す（例: `Modint9::mod() == 998244353`）。

### 関数一覧（動的 mod 側）

- `static void set_mod(int m)`  
  法を設定する。`Modint` を使う前に最初に呼ぶ。
- `static int mod()`  
  現在設定されている法を返す。

### 組合せユーティリティ `Binomial<Mint>`

`Mint` は `Modint9` / `Modint1` / `StaticModint<M>` / `DynamicModint<id>` を想定。

- `Binomial<Mint>()`  
  空で初期化（`0!` まで）。
- `explicit Binomial<Mint>(int n)`  
  `n` まで前計算して初期化。
- `explicit Binomial<Mint>(long long n)`  
  `long long` で受け取る版。内部で安全に `int` 範囲チェックを行う。
- `void ensure(int n)`  
  必要なら内部テーブルを `n` まで拡張。
- `void ensure(long long n)`  
  `long long` で受け取る版。`int` に収まらない場合は `assert` で停止。
- `Mint C(int n, int k)`  
  二項係数 `nCk`。`k < 0` または `k > n` は `0` を返す。
- `Mint C(long long n, long long k)`  
  `long long` で受け取る版。`int` 範囲外は `assert`。
- `Mint P(int n, int k)`  
  順列 `nPk`。`k < 0` または `k > n` は `0` を返す。
- `Mint P(long long n, long long k)`  
  `long long` で受け取る版。`int` 範囲外は `assert`。
- `Mint H(int n, int k)`  
  重複組合せ `nHk`。`n == 0` のときは `k == 0` のみ `1`。
- `Mint H(long long n, long long k)`  
  `long long` で受け取る版。`n + k - 1` が `long long` でオーバーフローする場合は `assert`。

### `Binomial` 使用例

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ

int main() {
    long long D = 200000;
    Binomial<Modint9> cb(D);

    Modint9 a = cb.C(D, 1000);
    Modint9 b = cb.P(D, 3);
    Modint9 c = cb.H(5, 2); // 5H2 = 15
    Modint9 z = cb.C(D, -1); // 0（範囲外は安全に0）
}
```

`C/P/H` に `long long` を直接渡せるが、  
内部実装は階乗テーブルを `vector` で持つため、実用上の上限は `int` 範囲内。

### メソッド例（pow / inv / val）

```C++
// 1) Modint.hpp の BEGIN/END マーカー範囲をここにコピペ

int main() {
    // 固定 mod
    Modint9 a = 2;
    Modint9 p = a.pow(10); // 2^10 = 1024
    Modint9 ia = a.inv();  // 2^{-1}
    Modint9 one = a * ia;  // 1
    int pv = p.val();              // 1024

    // 動的 mod
    Modint::set_mod(1000000007);
    Modint b = 3;
    Modint q = b.pow(5); // 243
    int qv = q.val();
}
```

### 注意

- `Modint`（動的 mod）は `set_mod` を呼ぶ前に使わない。
- `pow(n)` の `n` は `0` 以上を渡す。
- 除算は逆元を使うため、割る値と `mod` が互いに素である必要がある。

