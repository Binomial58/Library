---
title: Trie
documentation_of: ./trie.hpp
---

## Trie

```C++
namespace trie_lib
template <int ALPHABET = 26, char BASE = 'a'>
class Trie
```

文字列集合を管理するトライ木の最小限の骨組み。挿入・検索・接頭辞ごとの通過数カウントのみを提供し、
問題ごとの探索ロジック（LCP・XOR最大化など）は `node()` を使って呼び出し側で書く前提。

### 使い方

```C++
trie_lib::Trie<> trie; // 既定: 英小文字26種 ('a'-'z')
trie.insert("abc");
trie.insert("abb");

trie.contains("abc");   // true  (ちょうど挿入されている)
trie.contains("ab");    // false (途中までしか一致しない)
trie.has_prefix("ab");  // true  (abで始まる文字列が存在する)

// 自前の探索ロジックの例（ABC287E: 他の文字列との最長共通接頭辞）
int cur = trie.root();
int ans = 0;
for (int i = 0; i < (int)s.size(); i++)
{
    cur = trie.node(cur).children[s[i] - 'a'];
    if (trie.node(cur).prefix_count >= 2) ans = std::max(ans, i + 1);
}
```

01-trie（XOR最大化などに使う）にしたい場合はテンプレート引数を変える。

```C++
trie_lib::Trie<2, '0'> bit_trie; // 分岐数2、'0'/'1'の2文字を扱う
```

### 関数一覧

**コンストラクタ**
```C++
Trie()
```
根 (`nodes_[0]`) のみを持つ空のトライ木を作る。

**reserve**
```C++
void trie.reserve(int n)
```
ノード数の目安（= 挿入する文字列の長さの総和 + 1）が事前に分かっている場合、
`vector` の再割り当てを避けるために呼ぶ。省略可。

**insert**
```C++
int trie.insert(const std::string &s)
```
文字列 `s` を挿入し、終端ノードの番号を返す。挿入経路上の全ノードの `prefix_count` を+1する。

**find_node**
```C++
int trie.find_node(const std::string &s) const
```
`s` を辿った先のノード番号を返す。途中で辿れなくなったら `-1`。

**contains**
```C++
bool trie.contains(const std::string &s) const
```
`s` がちょうど挿入されている文字列と一致するか（`is_end` まで確認）。

**has_prefix**
```C++
bool trie.has_prefix(const std::string &s) const
```
`s` を接頭辞に持つ文字列が1つ以上挿入されているか。

**node / root / size**
```C++
const Node &trie.node(int i) const
int trie.root() const
int trie.size() const
```
ノード番号からノードの中身（`children` / `is_end` / `prefix_count`）を参照する。
問題ごとの探索ロジックはこれらを使って呼び出し側で組み立てる。

### 計算量

- `insert` / `find_node` / `contains` / `has_prefix`: $O(|s|)$
- 全体のノード数・メモリは $O(\Sigma |S_i| \times \mathrm{ALPHABET})$

### よくあるミス

- `Node` の `children` を `-1` で初期化し忘れると、未初期化値が根(0)などの実在ノード番号と
  誤認識されて誤った経路を辿ってしまう（このライブラリではコンストラクタで `fill(-1)` 済み）。
- `insert` で終端の文字も含めて `prefix_count` をインクリメントしているので、
  ある文字列が別の文字列の接頭辞になっているケース（例: `"abra"` と `"abracadabra"`）も
  特別扱いなく正しく処理できる。逆に、終端だけ数えたい／通過数を数えたくない場合は
  カウント対象を調整すること。

### Trie とは？

文字列の集合を、共通の接頭辞を共有する木構造として管理するデータ構造。
根から葉に向かって1文字ずつ辺をたどることで、ある文字列が集合に含まれるか、
ある接頭辞を持つ文字列が何個あるか、などを $O(|s|)$ で判定できる。

典型的な用途は、接頭辞に関するクエリ（今回のLCP最大値など）、01-trieによるXOR最大化、
複数パターン文字列検索（Aho-Corasickの土台）など。
