---
title: CombinatorialEnumeration
documentation_of: ./combinatorial_enumeration.hpp
---

## 組み合わせ・順列の全列挙
```C++
combinatorial_enumeration_lib
```

`std::vector<T>`（値列）または `0..n-1`（添字列）を対象に、組み合わせ・順列を列挙する。

- `combinations` / `permutations`: 全件を `std::vector<std::vector<...>>` で返す
- `for_each_combination` / `for_each_permutation`: 1件ずつコールバックに渡す

### 使い方（返り値で全件を受け取る）
```C++
#include "library/cpp/combinatorics/combinatorial_enumeration.hpp"

std::vector<int> a = {10, 20, 30};

auto c = combinatorial_enumeration_lib::combinations(a, 2);
// {{10,20}, {10,30}, {20,30}}

auto p = combinatorial_enumeration_lib::permutations(a, 2);
// {{10,20}, {10,30}, {20,10}, {20,30}, {30,10}, {30,20}}

auto full = combinatorial_enumeration_lib::permutations(a);
// 長さ3の順列を全列挙
```

### 使い方（for_each で1件ずつ処理）
`for_each_*` は「組み合わせ/順列1件ごとの foreach」を行うためのAPI。

```C++
std::vector<int> a = {10, 20, 30};

long long sum_c = 0;
combinatorial_enumeration_lib::for_each_combination(
    a, 2, [&](const std::vector<int>& c) {
        // c は今見ている1件
        for (int x : c) { // ここは通常の range-based for（foreach）
            sum_c += x;
        }
    });
```

### 添字（0..n-1）を直接列挙する例
```C++
auto c = combinatorial_enumeration_lib::combinations(5, 3);
// 0..4 から 3個選ぶ

auto p = combinatorial_enumeration_lib::permutations(5, 2);
// 0..4 から 2個並べる
```

### 添字 for_each の実戦例（Typical90 055 の形）
```C++
long long ans = 0;
long long p = /* mod */;
long long q = /* target */;
std::vector<long long> A = /* values */;

combinatorial_enumeration_lib::for_each_combination(
    (int)A.size(), 5, [&](const std::vector<int>& idx) {
        long long now = 1;
        for (int i : idx) {
            now = (now * A[i]) % p;
        }
        if (now == q) ++ans;
    });
```

### for_each_permutation の例
```C++
std::vector<char> ch = {'A', 'B', 'C'};

combinatorial_enumeration_lib::for_each_permutation(
    ch, 2, [&](const std::vector<char>& perm) {
        // {'A','B'}, {'A','C'}, {'B','A'}, ...
        // 必要な処理を書く
    });
```

### 関数一覧

**combinations**
```C++
template <class T>
std::vector<std::vector<T>> combinations(const std::vector<T>& items, int r)

std::vector<std::vector<int>> combinations(int n, int r)
```
`items` から `r` 個選ぶ全組み合わせを返す。  
`r < 0` または `r > n` のときは空配列を返す。

**for_each_combination（逐次処理）**
```C++
template <class T, class Callback>
void for_each_combination(const std::vector<T>& items, int r, Callback&& callback)

template <class Callback>
void for_each_combination(int n, int r, Callback&& callback)
```
各組み合わせを1件ずつ `callback(const std::vector<T>&)` に渡す。  
全件を `vector<vector<...>>` に保持しない。

**permutations（長さ指定）**
```C++
template <class T>
std::vector<std::vector<T>> permutations(const std::vector<T>& items, int r)

std::vector<std::vector<int>> permutations(int n, int r)
```
`items` から `r` 個並べる全順列を返す。  
`r < 0` または `r > n` のときは空配列を返す。

**for_each_permutation（逐次処理・長さ指定）**
```C++
template <class T, class Callback>
void for_each_permutation(const std::vector<T>& items, int r, Callback&& callback)

template <class Callback>
void for_each_permutation(int n, int r, Callback&& callback)
```
各順列を1件ずつ `callback(const std::vector<T>&)` に渡す。  
全件を `vector<vector<...>>` に保持しない。

**permutations（全長）**
```C++
template <class T>
std::vector<std::vector<T>> permutations(const std::vector<T>& items)

std::vector<std::vector<int>> permutations(int n)
```
`items.size()`（または `n`）を長さとする全順列を返す。

**for_each_permutation（逐次処理・全長）**
```C++
template <class T, class Callback>
void for_each_permutation(const std::vector<T>& items, Callback&& callback)

template <class Callback>
void for_each_permutation(int n, Callback&& callback)
```
長さ `items.size()`（または `n`）の順列を1件ずつ処理する。

**count_combinations / count_permutations**
```C++
std::size_t count_combinations(int n, int r)
std::size_t count_permutations(int n, int r)
```
それぞれ件数（`nCr`, `nPr`）を返す。  
内部で `reserve` に使っているため、返り値APIの再確保を減らせる。

### 計算量

- `combinations(items, r)`: `O(nCr * r)`
- `permutations(items, r)`: `O(nPr * r)`
- `for_each_combination(items, r, callback)`: `O(nCr * r)`
- `for_each_permutation(items, r, callback)`: `O(nPr * r)`

※ `combinations` / `permutations` は全列挙結果を保持するため、結果サイズぶんのメモリが必要。  
`for_each_*` は1件ずつ処理するため、作業用配列ぶん（`O(r)`）で使える。

### 注意

- 入力に同じ値が複数ある場合、値として同一の並びが複数回出ることがある（位置を区別して列挙するため）。
- `r == 0` のときは空列1つ（`{{}}`）を返す。
- `for_each_*` の引数ベクタは内部作業バッファが再利用される。コールバック外に保持したい場合は `saved.push_back(v);` のようにコピーする。
- `for_each_*` は早期終了（break）APIを持たない。途中で打ち切りたい場合は返り値版を使って自前ループで制御する。

