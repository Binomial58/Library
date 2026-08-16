---
title: EratosthenesSieve
documentation_of: ./eratosthenes_sieve.hpp
---

## エラトステネスの篩
```C++
sieve_lib::EratosthenesSieve
```

`0..max_n` の範囲で素数情報と最小素因数（Smallest Prime Factor, SPF）を前計算する。

SPFにより、`max_n` 以下の数を `O(log x)` で高速に素因数分解できる。N個の数（すべて `max_n` 以下）を素因数分解したい場面（例: abc177_e）では、Pollard's Rho（1つの大きな数向け、定数倍が重い）を使うより、こちらの `factorize` を使う方が適している。

### 使い方
```C++
#include "library/cpp/math/eratosthenes_sieve.hpp"

sieve_lib::EratosthenesSieve sieve(30);
bool p = sieve.is_prime(29);     // true
auto ps = sieve.primes();        // 2,3,5,7,11,13,17,19,23,29

auto factors = sieve.factorize(12); // {(2,2),(3,1)}
auto divs = sieve.divisors(12);     // {1,2,3,4,6,12}
```

### 関数一覧

**コンストラクタ**
```C++
sieve_lib::EratosthenesSieve sieve(int max_n)
```
`0..max_n` で篩を構築する。

**build**
```C++
void sieve.build(int max_n)
```
`0..max_n` で篩を再構築する。

**is_prime**
```C++
bool sieve.is_prime(int x)
```
`x` が素数なら `true`。  
範囲外（`x < 0` または `x > max_n`）は `false`。

**primes**
```C++
const std::vector<int>& sieve.primes()
```
構築範囲内の素数一覧を昇順で返す。

**max_n**
```C++
int sieve.max_n()
```
現在の構築上限を返す。

**smallest_prime_factor**
```C++
int sieve.smallest_prime_factor(ll x)
```
`x`（`2 <= x <= max_n`）の最小の素因数を返す。  
範囲外・`x < 2` のときは `0` を返す。

**factorize**
```C++
std::vector<std::pair<ll, int>> sieve.factorize(ll x)
```
`x`（`1 <= x <= max_n`）を素因数分解し、`(素数, 指数)` のペアを素数の昇順で返す。  
`x == 1` のときは空の配列。範囲外（`x < 1` または `x > max_n`）も空の配列を返す。

**divisors**
```C++
std::vector<ll> sieve.divisors(ll x)
```
`x`（`1 <= x <= max_n`）の約数を昇順で列挙する。`factorize` を利用して計算する。  
範囲外は空の配列を返す。

### 計算量

- `build`: `O(n log log n)`（SPFの計算を含めても定数倍程度で変わらない）
- `is_prime`: `O(1)`
- `primes`: `O(1)`（参照取得）
- `smallest_prime_factor`: `O(1)`
- `factorize(x)`: `O(log x)`（`x` の素因数の個数、重複込みに比例）
- `divisors(x)`: `factorize` の結果から約数を構築する分だけかかる（約数の個数を `d(x)` として `O(d(x) log x)` 程度）

