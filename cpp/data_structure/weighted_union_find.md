---
title: WeightedUnionFind
documentation_of: ./weighted_union_find.hpp
---

## WeightedUnionFind
```C++
template <class W = long long>
class WeightedUnionFind
```

各要素 `x` に潜在的な値（ポテンシャル）`X_x` があると考え、「`X_a - X_b = w`」という制約を逐次追加していきながら、制約同士が矛盾しないか判定し、同じ集合に属する2要素間のポテンシャル差を取得できる。[UnionFind](./union_find.md) に「各要素が根から見てどれだけズレているか」を表す配列を追加した拡張版。

`WeightedGraph<W = long long>` と同じ形式でテンプレート化してある（ポテンシャルの型が `int` で足りるケースは稀だが、汎用性のため）。

### 使い方

```C++
WeightedUnionFind<ll> uf(n);
if (uf.unite(a, b, w)) { // X_a - X_b = w を追加
    // 矛盾なし
}
if (uf.same(a, b)) {
    ll d = uf.diff(a, b); // X_a - X_b
}
```

### 関数一覧

**コンストラクタ**
```C++
WeightedUnionFind<W> uf(int n)
```
0-indexed で `n` 要素を作る。各要素は最初バラバラの集合（自分自身のポテンシャルを `0` とする）。

**find**
```C++
int uf.find(int x)
```
`x` の根を返す。経路圧縮を行う。

**weight**
```C++
W uf.weight(int x)
```
`find(x)` で圧縮したうえで、`X_x - X_{root(x)}` を返す。

**same**
```C++
bool uf.same(int a, int b)
```
`a`, `b` が同じ集合か。

**unite**
```C++
bool uf.unite(int a, int b, W w)
```
`X_a - X_b = w` という制約を追加する。

- 矛盾がなければ `true`（新規併合でも、既存の関係と整合していただけでも `true`）
- 矛盾していれば `false`（何も変更しない）

**diff**
```C++
W uf.diff(int a, int b)
```
`a`, `b` が同じ集合であることを前提に `X_a - X_b` を返す（`weight(a) - weight(b)`）。異なる集合の場合は `assert` で停止する。

**size / roots / groups**
```C++
int uf.size(int x)
std::vector<int> uf.roots()
std::vector<std::vector<int>> uf.groups()
```
[UnionFind](./union_find.md) と同じ。ポテンシャルには影響しない。

### 計算量

- `find` / `weight` / `same` / `unite` / `diff`: ならし `O(α(N))`
- `roots` / `groups`: 全体で `O(N α(N))`

### 実装のハマりどころ

**`find` の経路圧縮順序** — `weight_[x]`（x から直接の親までの差分）を、親を根まで圧縮し終える**前**に確定させると、祖父より上の分がズレたまま反映されないバグになる。必ず「親を再帰的に圧縮する（`find(親)` を呼ぶ）→ 圧縮済みの `weight_[親]` を使って `weight_[x]` を更新する」の順で行うこと。

**`unite` での a, b の扱い** — `a`, `b` を根のインデックスで上書きする**前**に、`weight(a)`, `weight(b)` を呼んで元の `a`, `b` の「根から見た差分」を控えておくこと。先に `a = find(a); b = find(b);` としてしまうと、`a == b`（同じ集合）になった場合に元の情報が失われ、常に差分 `0` と比較する結果になってしまう。

根が異なる場合、`w' = w - weight(a) + weight(b)`（＝ `X_{root(a)} - X_{root(b)}`）を計算し、union by size で付け替える側（小さい木の根）の `weight_` に、付け替え方向に応じた符号（swap した場合は `w'` の符号を反転）で代入する。

### 検証

コミットには含めていないが、以下をローカルで確認済み:
- 仕様書の基本例（連鎖的な `unite`/`diff`）
- 矛盾する制約を `unite` した際に `false` を返し、状態が変更されないこと
- `unite(a, a, 0)` のような自明なケースの整合性判定
- 200要素の鎖状データでの経路圧縮の正しさ（多段の圧縮を経ても差分が壊れないか）
- BFS ベースのナイーブ参照実装との突き合わせによる300試行×100操作のランダムストレステスト（`unite` の成否、`same`、`diff` を毎回全ペアでクロスチェック）
- `diff` を異なる集合の要素に対して呼んだときに `assert` で停止すること

### 典型的な使い方

ABC328F のような、`X_a - X_b = d` の制約を順に試し、矛盾しなければ採用する差分制約の整合性判定・オンライン処理に使う。

XOR 版（`X_a XOR X_b = d`、ABC396E で使用）も同じ骨格で `+` を `XOR` に、符号反転を「同じ XOR をもう一度適用」に置き換えるだけで作れる（このライブラリでは未実装）。
