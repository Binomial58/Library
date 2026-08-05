---
title: BaseConversion
documentation_of: ./base_conversion.hpp
---

## 進数変換（k -> 10, 10 -> n）
```C++
base_conversion_lib
```

`k` 進数文字列を 10 進（`unsigned long long`）へ安全に変換し、  
10 進（`unsigned long long`）を `n` 進数文字列へ変換する。

- 対応基数: `2..36`
- 使用可能な桁文字: `0-9`, `A-Z`, `a-z`
- `k -> 10` はオーバーフロー検出あり

### 使い方
```C++
#include "library/cpp/math/base_conversion.hpp"

auto r = base_conversion_lib::k_to_10("177", 8);
if (!r) {
    // 失敗時は r.status を確認
}
unsigned long long x = r.value;          // 127
std::string s = base_conversion_lib::ten_to_n(x, 9); // "151"
```

### オーバーフロー検出の例
```C++
auto r = base_conversion_lib::k_to_10("18446744073709551616", 10);
if (r.status == base_conversion_lib::ParseStatus::overflow) {
    // unsigned long long の範囲外
}
```

### 関数一覧

**ParseStatus**
```C++
enum class ParseStatus {
    ok,
    invalid_base,
    empty_input,
    invalid_digit,
    overflow,
};
```
`k_to_10` の結果状態を表す。

**ParseU64Result**
```C++
struct ParseU64Result {
    ParseStatus status;
    unsigned long long value;
    std::size_t error_pos;
    explicit operator bool() const;
};
```
- `status == ok` のとき `value` が有効
- `error_pos` は失敗位置（該当なしは `std::string_view::npos`）

**is_valid_base**
```C++
bool is_valid_base(int base)
```
基数が `2..36` なら `true`。

**k_to_10（詳細結果版）**
```C++
ParseU64Result k_to_10(std::string_view s, int base)
```
`base` 進数文字列 `s` を 10 進 `unsigned long long` に変換する。  
失敗時は `status` で理由を返す。

**k_to_10（bool版）**
```C++
bool k_to_10(std::string_view s, int base, unsigned long long& out)
```
成功時 `out` に値を入れて `true`、失敗時 `false`。

**ten_to_n（戻り値文字列版）**
```C++
std::string ten_to_n(unsigned long long value, int base)
```
10 進 `value` を `base` 進文字列へ変換する。  
`base` が不正なときは空文字列を返す。

**ten_to_n（bool版）**
```C++
bool ten_to_n(unsigned long long value, int base, std::string& out)
```
成功時 `out` に結果を入れて `true`、失敗時 `false`。

**parse_status_message**
```C++
const char* parse_status_message(ParseStatus status)
```
`ParseStatus` を文字列へ変換する（デバッグ用）。

### 計算量

- `k_to_10(s, base)`: `O(|s|)`
- `ten_to_n(value, base)`: `O(log_base(value + 1))`

### 注意

- このライブラリの `k -> 10` は `unsigned long long` 範囲内のみ成功する。
- 負数（先頭 `-`）は扱わない。
- `ten_to_n(0, base)` の結果は `"0"`。

