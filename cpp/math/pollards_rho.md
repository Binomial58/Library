---
title: PollardRho
documentation_of: ./pollards_rho.hpp
---

## PollardRho
```C++
namespace pollards_rho
```

### 使い方
```C++
auto mp = pollards_rho::factorize(n); // map<素因数, 指数>
auto ds = pollards_rho::divisors(n);  // 約数（昇順）
bool ok = pollards_rho::is_prime(n);
```

### 関数一覧

**素数判定**
```C++
bool pollards_rho::is_prime(unsigned long long n)
```
64bit 整数の Miller-Rabin（決定版）。

**素因数分解**
```C++
std::map<unsigned long long, int>
pollards_rho::factorize(unsigned long long n)
```
`n` を素因数分解して `map<素因数, 指数>` を返す。`n <= 1` のときは空。

**約数列挙**
```C++
std::vector<unsigned long long>
pollards_rho::divisors(unsigned long long n)
```
`n` の約数を昇順で返す。`n == 0` のときは空。

**1 つ因数を探す**
```C++
unsigned long long pollards_rho::find_prime_factor(unsigned long long n)
```
`n` の非自明因子を 1 つ返す（`n` が素数なら `n`）。

**内部ユーティリティ**
```C++
unsigned long long pollards_rho::modpow(unsigned long long a, unsigned long long e,
                                        unsigned long long mod)
unsigned long long pollards_rho::modmul(unsigned long long a, unsigned long long b,
                                        unsigned long long mod)
unsigned long long pollards_rho::gcd(unsigned long long a, unsigned long long b)
```

### 計算量の目安
- `is_prime`: $ O(\log n) $ 程度（定数は小さめ）
- `factorize`: 期待 $ O(n^{1/4}) $
- `divisors`: 期待 $ O(n^{1/4} + d(n)) $（$d(n)$ は約数の個数）

### Pollard’s Rho とは？
大きな整数の素因数分解を高速に行う確率的アルゴリズム。  
Miller-Rabin と組み合わせて 64bit 整数の分解に使われることが多い。

