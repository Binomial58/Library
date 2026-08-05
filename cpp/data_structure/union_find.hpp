#pragma once

#include <utility>
#include <vector>

class UnionFind
{
public:
    // n要素のUnion-Findを作る（0-indexed）
    UnionFind() = default;
    explicit UnionFind(int n) : parent_or_size_(n, -1) {}

    // xの根（代表）を返す（経路圧縮あり）
    int find(int x)
    {
        if (parent_or_size_[x] < 0)
            return x;
        return parent_or_size_[x] = find(parent_or_size_[x]);
    }

    // aとbが同じ集合か
    bool same(int a, int b)
    {
        return find(a) == find(b);
    }

    // aとbを併合する（既に同じならfalse）
    bool unite(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b)
            return false;
        // union by size (negative size)
        if (parent_or_size_[a] > parent_or_size_[b])
            std::swap(a, b);
        parent_or_size_[a] += parent_or_size_[b];
        parent_or_size_[b] = a;
        return true;
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
};
