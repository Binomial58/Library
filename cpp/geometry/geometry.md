---
title: Geometry
documentation_of: ./geometry.hpp
---

## Geometry

2 次元幾何の自作ライブラリ。
`std::complex<double>` を点として使い、線分交差・距離・円・凸多角形まわりを一通りまとめている。

### 使い方

```cpp
#include "geometry.hpp"
using namespace geometry;

int main() {
    Point a(0, 0), b(2, 0), p(1, 1);
    double d = dist_sp(a, b, p);
}
```

### 基本型

```cpp
using Real = double;
using Point = std::complex<Real>;
using Line = std::pair<Point, Point>;
using Segment = std::pair<Point, Point>;
using Polygon = std::vector<Point>;

struct Circle {
    Point center;
    Real radius;
};
```

### Point の宣言方法

`Point` は `namespace geometry` の中で定義された型なので、そのまま `Point p(x, y);` と書くとコンパイルエラーになる（`error: 'Point' was not declared in this scope`）。次のどちらかが必要。

```cpp
// 方法1: using namespace で名前空間ごと持ち込む（ファイルの先頭などで1回書く）
using namespace geometry;
Point p(1, 2);

// 方法2: 毎回 geometry:: を付けて明示する（using namespace を書きたくない場合）
geometry::Point p(1, 2);
```

どちらか一方があればよく、両方書く必要はない。`using namespace geometry;` を書いた後は、`Point` だけでなく `Line` や `Segment`、`ccw` や `dist_sp` などの関数も `geometry::` なしで使えるようになる。

**座標の取り出し方**

`Point` の実体は `std::complex<Real>`（`Real = double`）なので、メンバは `.x`/`.y` ではなく、`std::complex` の `.real()`/`.imag()` を使う。

```cpp
Point p(3, 4);
double x = p.real(); // 3
double y = p.imag(); // 4
```

**整数座標からの構築**

コンストラクタは `double` を受け取るが、`ll`（`long long`）などの整数を渡しても暗黙変換されるのでそのまま使える。

```cpp
LL(ax, ay);              // ax, ay は ll 型
Point a(ax, ay);         // OK。ax, ay は double に暗黙変換される
```

### 命名規則

関数名の末尾は `_XY` や `_XYZ` のように、**引数の図形の種類を表す文字を、渡す順番通りに並べたもの**になっている。

| 文字 | 意味 | 表現方法 |
|---|---|---|
| `p` | 点 (Point) | 1点 |
| `l` | 直線 (Line) | 2点 `a1, a2` を通る、**両方向に無限に延びる**直線 |
| `s` | 線分 (Segment) | 2点 `a1, a2` を端点とする、**そこで途切れる**線分 |
| `c` | 円 (Circle) | 中心 `c` と半径 `r`（または `Circle` 構造体） |

例えば `dist_lp(a1, a2, p)` は「`a1,a2`が表す**直線**」と「点`p`」の距離、`is_intersect_ss(a1, a2, b1, b2)` は「線分`a1-a2`」と「線分`b1-b2`」が交差するかどうかを表す。`dist_lc`/`dist_sc`/`dist_cc` のような円がらみの関数にも同じ規則が使われている（`c` = 円）。

**`l`（直線）と`s`（線分）の違いに注意**

同じ2点 `a1, a2` を渡していても、`l`系の関数はその2点を通る直線が両方向に無限に延びているとみなし、`s`系の関数はその2点そのものが端点だとみなす。そのため、**同じ入力でも`_ll`系と`_ss`系で結果が変わることがある**。

例：

```cpp
Point a1(0, 0), a2(1, 0);   // x軸上、x=0〜1の線分
Point b1(5, -1), b2(5, 1);  // x=5の垂直な線分（x=5, y=-1〜1）

is_intersect_ll(a1, a2, b1, b2); // true  （直線同士はx=5で交わる）
is_intersect_ss(a1, a2, b1, b2); // false （線分a1-a2はx=0〜1までしかなく、x=5に届かない）
```

つまり、

- `l`系：「2点を通る無限の直線」として振る舞う（平行でなければほぼ必ず交わる・距離0になりやすい）
- `s`系：「2点を端点とする実際の線分」として振る舞う（届かなければ交わらない・距離が0にならないことが多い）

どちらを使うべきかは問題設定次第（例えば「レーザーが板を突き抜けるか」なら `l`、「実際に引かれた線分同士が交わるか」なら `s`）。

### 基本関数

**`sgn` / `eq`**

```cpp
int sgn(Real x);
bool eq(Real a, Real b);
bool eq(Point a, Point b);
```

EPS 付き比較。

**`dot` / `cross` / `norm2`**

```cpp
Real dot(Point a, Point b);
Real cross(Point a, Point b);
Real norm2(Point a);
```

内積・外積・二乗ノルム。

**`rotate90` / `rotate`**

```cpp
Point rotate90(Point a);
Point rotate(Point a, Real rad);
```

点を 90 度回転、または任意角回転する。

**`ccw`**

```cpp
int ccw(Point a, Point b, Point c);
```

返り値の意味:

- `+1`: counter clockwise
- `-1`: clockwise
- `+2`: `c-a-b`
- `-2`: `a-b-c`
- `0`: `a-c-b`

### 交差判定

```cpp
bool is_intersect_lp(Point a1, Point a2, Point p);              // 直線と点
bool is_intersect_ll(Point a1, Point a2, Point b1, Point b2);   // 直線と直線
bool is_intersect_ls(Point a1, Point a2, Point b1, Point b2);   // 直線と線分
bool is_intersect_ss(Point a1, Point a2, Point b1, Point b2);   // 線分と線分
bool is_intersect_sp(Point a1, Point a2, Point p);              // 線分と点
```

接する場合も交差とみなす。

### 射影・反射・距離

```cpp
Point projection(Point a1, Point a2, Point p);
Point reflection(Point a1, Point a2, Point p);

Real dist_lp(Point a1, Point a2, Point p);
Real dist_ll(Point a1, Point a2, Point b1, Point b2);
Real dist_ls(Point a1, Point a2, Point b1, Point b2);
Real dist_sp(Point a1, Point a2, Point p);
Real dist_ss(Point a1, Point a2, Point b1, Point b2);
```

**`crosspoint_ll`**

```cpp
Point crosspoint_ll(Point a1, Point a2, Point b1, Point b2);
```

2 直線の交点を返す。平行で交点がない場合は例外を投げる。

### 円

```cpp
Real dist_lc(Point a1, Point a2, Point c, Real r);
Real dist_sc(Point a1, Point a2, Point c, Real r);
std::vector<Point> crosspoint_lc(Point a1, Point a2, Point c, Real r);
Real dist_cc(Point a, Real ar, Point b, Real br);
std::vector<Point> crosspoint_cc(Point a, Real ar, Point b, Real br);
std::vector<Point> tangent_points(Point c, Real r, Point p);
std::vector<Line> tangent_lines(Circle a, Circle b);
Point circumcenter(Point a, Point b, Point c);
std::vector<Point> circles_points_radius(Point a, Point b, Real r);
Point minimum_enclosing_circle(const Polygon& ps);
```

よく使う交点、接線、外接円中心をまとめてある。

### 多角形

```cpp
Polygon convex_hull(Polygon ps);
bool is_ccw_convex(const Polygon& ps);
Containment in_convex(Point p, const Polygon& ps);
Containment in_ccw_convex(const Polygon& ps, Point p);
Containment in_polygon(const Polygon& ps, Point p);
Polygon convex_cut(const Polygon& ps, Point a1, Point a2);
std::pair<int, int> convex_diameter(const Polygon& ps);
Real area(const Polygon& ps);
Point centroid(const Polygon& ps);
Polygon voronoi_cell(Point p, const Polygon& sites, const Polygon& outer);
```

`Containment` は以下:

```cpp
enum Containment { OUT = 0, IN = 1, ON = 2 };
```

### 幾何グラフ・補助

```cpp
GeoGraph segment_arrangement(const std::vector<Segment>& segs, Polygon& ps);
GeoGraph visibility_graph(const Polygon& ps, const std::vector<Polygon>& objs);
std::vector<Segment> merge_segments(std::vector<Segment> segs);
```

`segment_arrangement` は交点を頂点とするグラフを組み、`ps` に頂点集合を返す。

`GeoGraph`/`GeoEdge` は幾何用の重み付きグラフ型（`edges[v]` に `GeoEdge{from, to, cost}` を持つ）。テンプレートのグローバルな `Graph`/`WeightedGraph` とは別物で、`using namespace geometry;` しても衝突しない。

### 計算量

特記のない関数はすべて `O(1)`。

**基本関数・交差判定・射影反射距離・円**

`sgn`, `eq`, `dot`, `cross`, `norm2`, `rotate90`, `rotate`, `ccw`,
`is_intersect_lp/ll/ls/ss/sp`,
`projection`, `reflection`, `dist_lp/ll/ls/sp/ss`, `crosspoint_ll`,
`dist_lc/sc/cc`, `crosspoint_lc/cc`, `tangent_points`, `tangent_lines`, `circumcenter`, `circles_points_radius`

— いずれも `O(1)`。

**多角形**（`n` はその関数に渡す点列のサイズ）

- `convex_hull(ps)`: `O(n log n)`（ソート + Andrew のモノトーンチェーン法）
- `is_ccw_convex(ps)` / `in_convex(p, ps)` / `in_polygon(ps, p)` / `convex_cut(ps, a1, a2)` / `convex_diameter(ps)` / `area(ps)` / `centroid(ps)`: `O(n)`
  - `in_convex`・`convex_diameter` は `ps` が凸多角形（反時計回り）であることが前提
- `in_ccw_convex(ps, p)`: `O(log n)`。`ps` が凸多角形（反時計回り）であることを前提にした二分探索
- `minimum_enclosing_circle(ps)`: `O(n)`（定数倍が大きい: `39 × 50 = 1950` 回の反復で中心を動かす近似解法。Welzl の乱択増分法のような厳密アルゴリズムではないので、精度は反復回数に依存する）
- `voronoi_cell(p, sites, outer)`: 目安 `O(|sites| * (|outer| + |sites|))`。サイトごとに `convex_cut` を1回呼び、セルの頂点数は切断のたびに高々定数個ずつ増える

**幾何グラフ・補助**

- `segment_arrangement(segs, ps)`: 目安 `O(n^3)`（`n = segs.size()`）。全ペアの交点計算に `O(n^2)`、その後、各線分について交点候補（最大 `O(n^2)` 個）を線形走査するため。線分数が増えると重くなるので注意
- `visibility_graph(ps, objs)`: `O(n^2 * V)`（`n = ps.size()`、`V` は `objs` に含まれる頂点数の合計）。全点ペアについて、各障害物の内外判定・辺との交差判定（いずれも `O(|obj|)`）を行う
- `merge_segments(segs)`: `O(n^2)`（`n = segs.size()`）。全ペアを比較し、併合できたペアはその場で詰めて再走査する

### 典型コード

線分交差:

```cpp
if (is_intersect_ss(a, b, c, d)) {
    auto p = crosspoint_ll(a, b, c, d);
}
```

凸包:

```cpp
Polygon hull = convex_hull(points);
double s = area(hull);
```

点の包含:

```cpp
if (in_polygon(poly, q) != OUT) {
    // 内部または辺上
}
```

