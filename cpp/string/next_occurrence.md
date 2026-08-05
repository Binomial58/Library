---
title: NextOccurrence
documentation_of: ./next_occurrence.hpp
---

# Next Occurrence

文字列に対して、「ある位置以降で、各文字が次にどこに出現するか」を前処理するライブラリ。

典型的には `nex` や `next_pos` と呼ぶ。

```cpp
#include "library/cpp/string/next_occurrence.hpp"
```

## 何を持つか

文字列 `s` の長さを `n` とする。

```text
nex[i][c]
```

は、次の値を表す。

```text
位置 i 以降で、文字 char('a' + c) が最初に現れる index
```

存在しない場合は `n` が入る。

例えば、

```text
s = "abca"
```

なら、

```text
nex[0]['a' - 'a'] = 0
nex[1]['a' - 'a'] = 3
nex[2]['b' - 'a'] = 4  // 存在しないので n
nex[3]['a' - 'a'] = 3
nex[4]['a' - 'a'] = 4  // 末尾の番兵
```

このライブラリでは英小文字 `'a'` から `'z'` までの 26 種類を対象にしている。

## API

```cpp
auto nex = next_occurrence_lib::calc_next_occurrence(s);
```

`s` から次出現位置テーブルを作る。

短い別名も用意している。

```cpp
auto nex = next_occurrence_lib::calc_nex(s);
```

返り値の型は次の通り。

```cpp
std::vector<std::vector<int>>
```

サイズは `(n + 1) x 26`。

## 計算量

- 前処理: `O(26N)`
- メモリ: `O(26N)`
- 1回の参照: `O(1)`

`N = 100000` 程度なら十分軽い。

## 実装の考え方

後ろから見ていく。

位置 `i` 以降の情報は、まず位置 `i + 1` 以降の情報と同じ。

そのうえで、`s[i]` だけは位置 `i` に更新する。

```cpp
nex[i] = nex[i + 1];
nex[i][s[i] - 'a'] = i;
```

この考え方により、各位置について 26 文字分をコピーすればよい。

## 例: 部分列判定

文字列 `t` が `s` の部分列かどうかを判定する。

```cpp
bool is_subsequence(const string &s, const string &t)
{
    auto nex = next_occurrence_lib::calc_next_occurrence(s);
    int n = (int)s.size();
    int pos = 0;

    for (char ch : t)
    {
        int idx = nex[pos][ch - 'a'];
        if (idx == n)
            return false;
        pos = idx + 1;
    }

    return true;
}
```

`pos` は「次に探し始める位置」を表す。

文字 `ch` を `idx` で使ったら、次はその直後の `idx + 1` から探す。

## 例: 辞書順最小の長さ K の部分列

Typical90 006 の型。

辞書順最小を作るときは、答えを左から決める。

各桁で `'a'`, `'b'`, ..., `'z'` の順に試し、その文字を選んでも残り文字数が足りるなら採用する。

```cpp
string smallest_subsequence(const string &s, int k)
{
    int n = (int)s.size();
    auto nex = next_occurrence_lib::calc_next_occurrence(s);

    string ans;
    int pos = 0;

    for (int len = 0; len < k; ++len)
    {
        for (int c = 0; c < 26; ++c)
        {
            int idx = nex[pos][c];
            if (idx == n)
                continue;

            // idx の文字を選んだあと、必要な残り文字数を確保できるか
            if (n - idx >= k - len)
            {
                ans += char('a' + c);
                pos = idx + 1;
                break;
            }
        }
    }

    return ans;
}
```

判定式の意味:

```cpp
n - idx >= k - len
```

`idx` の文字を今選ぶと、`idx` から末尾までには `n - idx` 文字ある。

今から作る必要がある文字数は、自分自身を含めて `k - len` 文字。

したがって、この条件を満たすなら、その文字を選んでも最後まで作れる。

## 例: 文字列上の DP

`s` の部分列として、何らかの文字列を作っていく DP でも使える。

例えば状態を、

```text
今、s の pos 文字目以降を見ている
```

と持つと、文字 `c` を次に選ぶ遷移は、

```cpp
int idx = nex[pos][c - 'a'];
if (idx != n)
{
    int next_pos = idx + 1;
}
```

のように書ける。

これは「部分列オートマトン」のような見方もできる。

## よく使う場面

次のような言葉が問題文や考察に出たら候補になる。

- `S` の部分列として作れるか
- 文字を順番に選ぶ
- 次に現れる位置を何度も探す
- 辞書順最小の部分列
- 英小文字 26 種類
- 文字列上で状態が「今見ている位置」になる DP

特に、

```text
毎回 find すると O(N) かかる
同じ文字列に対して「次の c はどこか」を何度も聞きたい
```

という状況では `nex` が効く。

## 注意点

この実装は英小文字 26 種類専用。

数字や大文字、任意文字集合で使いたい場合は、次のどちらかにする。

- 文字種数に合わせて列数を変える
- 文字を座標圧縮して使う

また、存在しない場合は `-1` ではなく `n` が返る。

そのため、存在判定は次のように書く。

```cpp
if (nex[pos][c] == n)
{
    // not found
}
```

## 関連する典型

- Typical90 006: Smallest Subsequence
- 部分列判定
- 辞書順貪欲
- 文字列 DP
