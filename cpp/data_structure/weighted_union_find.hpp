#pragma once

#include <cassert>
#include <utility>
#include <vector>

template <class W = long long>
class WeightedUnionFind
{
public:
    // n要素の重み付きUnion-Findを作る（0-indexed）。各要素は最初バラバラの集合。
    WeightedUnionFind() = default;
    explicit WeightedUnionFind(int n) : parent_or_size_(n, -1), weight_(n, W(0)) {}

    // xの根（代表）を返す（経路圧縮あり）
    // 経路圧縮の際、weight_[x]（xから直接の親までの差分）は、
    // 親を根まで圧縮し終えたあと（=weight_[親]が根からの差分に更新されたあと）に加算する。
    // 先にweight_[x]を確定させてしまうと、祖父より上の分がズレたまま反映されないので注意。
    int find(int x)
    {
        if (parent_or_size_[x] < 0)
            return x;
        const int p = parent_or_size_[x];
        const int r = find(p);
        weight_[x] += weight_[p];
        parent_or_size_[x] = r;
        return r;
    }

    // X_x - X_{find(x)} を返す（xを圧縮したうえで求める）
    W weight(int x)
    {
        find(x);
        return weight_[x];
    }

    // aとbが同じ集合か
    bool same(int a, int b)
    {
        return find(a) == find(b);
    }

    // X_a - X_b = w という制約を追加する。
    // 矛盾がなければtrue（新規併合でも、既存の関係と整合していただけでもtrue）。
    // 矛盾していればfalseで何も変更しない。
    //
    // 実装上の注意：a, bを根のインデックスで上書きする前に weight(a), weight(b) を
    // 呼んで元のa, bの「根から見た差分」を控えておくこと。先に a = find(a); b = find(b);
    // としてしまうと、a == bになった場合に元の情報が失われ、常に差分0と比較してしまう。
    bool unite(int a, int b, W w)
    {
        const W wa = weight(a); // X_a - X_{root(a)}
        const W wb = weight(b); // X_b - X_{root(b)}
        int ra = find(a);
        int rb = find(b);
        if (ra == rb)
            return wa - wb == w; // X_a - X_b = wa - wb がwと矛盾しないか

        // X_a - X_b = w より X_{ra} - X_{rb} = w - wa + wb
        W d = w - wa + wb;
        // union by size (negative size)。付け替える側（小さい木の根）にdの符号を反転して代入する。
        if (parent_or_size_[ra] > parent_or_size_[rb])
        {
            std::swap(ra, rb);
            d = -d;
        }
        parent_or_size_[ra] += parent_or_size_[rb];
        parent_or_size_[rb] = ra;
        weight_[rb] = -d;
        return true;
    }

    // aとbが同じ集合であることを前提に X_a - X_b を返す
    W diff(int a, int b)
    {
        assert(same(a, b));
        return weight(a) - weight(b);
    }

    // xが属する集合のサイズ
    int size(int x)
    {
        return -parent_or_size_[find(x)];
    }

    // 各連結成分の根（代表）を列挙する
    std::vector<int> roots()
    {
        std::vector<int> res;
        const int n = static_cast<int>(parent_or_size_.size());
        res.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            if (find(i) == i)
                res.push_back(i);
        }
        return res;
    }

    // 連結成分ごとの要素一覧を返す
    std::vector<std::vector<int>> groups()
    {
        const int n = static_cast<int>(parent_or_size_.size());
        std::vector<int> leader_buf(n), group_size(n);
        for (int i = 0; i < n; ++i)
        {
            leader_buf[i] = find(i);
            ++group_size[leader_buf[i]];
        }

        std::vector<std::vector<int>> result(n);
        for (int i = 0; i < n; ++i)
        {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < n; ++i)
        {
            result[leader_buf[i]].push_back(i);
        }

        std::vector<std::vector<int>> res;
        res.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            if (!result[i].empty())
                res.push_back(std::move(result[i]));
        }
        return res;
    }

private:
    std::vector<int> parent_or_size_;
    std::vector<W> weight_;
};
