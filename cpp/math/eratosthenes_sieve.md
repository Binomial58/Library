---
title: EratosthenesSieve
documentation_of: ./eratosthenes_sieve.hpp
---

## エラトステネスの篩
```C++
sieve_lib::EratosthenesSieve
```

`0..max_n` の範囲で素数情報を前計算する。

### 使い方
```C++
#include "library/cpp/math/eratosthenes_sieve.hpp"

sieve_lib::EratosthenesSieve sieve(30);
bool p = sieve.is_prime(29);     // true
auto ps = sieve.primes();        // 2,3,5,7,11,13,17,19,23,29
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

### 計算量

- `build`: `O(n log log n)`
- `is_prime`: `O(1)`
- `primes`: `O(1)`（参照取得）

