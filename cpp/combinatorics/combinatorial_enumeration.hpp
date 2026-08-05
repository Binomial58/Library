#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace combinatorial_enumeration_lib
{
    inline std::size_t count_combinations(int n, int r)
    {
        if (r < 0 || r > n)
            return 0;
        r = std::min(r, n - r);
        __int128 ways = 1;
        const __int128 max_size = static_cast<__int128>(std::numeric_limits<std::size_t>::max());
        for (int i = 1; i <= r; ++i)
        {
            ways = ways * (n - r + i) / i;
            if (ways > max_size)
                return std::numeric_limits<std::size_t>::max();
        }
        return static_cast<std::size_t>(ways);
    }

    inline std::size_t count_permutations(int n, int r)
    {
        if (r < 0 || r > n)
            return 0;
        __int128 ways = 1;
        const __int128 max_size = static_cast<__int128>(std::numeric_limits<std::size_t>::max());
        for (int i = 0; i < r; ++i)
        {
            ways *= (n - i);
            if (ways > max_size)
                return std::numeric_limits<std::size_t>::max();
        }
        return static_cast<std::size_t>(ways);
    }

    template <class T>
    void dfs_combinations(const std::vector<T> &items, int idx, int r,
                          std::vector<T> &current, std::vector<std::vector<T>> &result)
    {
        if (r == 0)
        {
            result.push_back(current);
            return;
        }
        if (idx == static_cast<int>(items.size()))
            return;
        if (static_cast<int>(items.size()) - idx < r)
            return;

        current.push_back(items[idx]);
        dfs_combinations(items, idx + 1, r - 1, current, result);
        current.pop_back();

        dfs_combinations(items, idx + 1, r, current, result);
    }

    template <class T>
    std::vector<std::vector<T>> combinations(const std::vector<T> &items, int r)
    {
        std::vector<std::vector<T>> result;
        if (r < 0 || r > static_cast<int>(items.size()))
            return result;

        result.reserve(count_combinations(static_cast<int>(items.size()), r));
        std::vector<T> current;
        current.reserve(r);
        dfs_combinations(items, 0, r, current, result);
        return result;
    }

    template <class T, class Callback>
    void dfs_combinations_callback(const std::vector<T> &items, int idx, int r,
                                   std::vector<T> &current, Callback &callback)
    {
        if (r == 0)
        {
            callback(current);
            return;
        }
        if (idx == static_cast<int>(items.size()))
            return;
        if (static_cast<int>(items.size()) - idx < r)
            return;

        current.push_back(items[idx]);
        dfs_combinations_callback(items, idx + 1, r - 1, current, callback);
        current.pop_back();

        dfs_combinations_callback(items, idx + 1, r, current, callback);
    }

    template <class T, class Callback>
    void for_each_combination(const std::vector<T> &items, int r, Callback &&callback)
    {
        if (r < 0 || r > static_cast<int>(items.size()))
            return;
        auto cb = std::forward<Callback>(callback);
        std::vector<T> current;
        current.reserve(r);
        dfs_combinations_callback(items, 0, r, current, cb);
    }

    template <class T>
    void dfs_permutations(const std::vector<T> &items, int r, std::vector<char> &used,
                          std::vector<T> &current, std::vector<std::vector<T>> &result)
    {
        if (static_cast<int>(current.size()) == r)
        {
            result.push_back(current);
            return;
        }

        for (int i = 0; i < static_cast<int>(items.size()); ++i)
        {
            if (used[i])
                continue;
            used[i] = 1;
            current.push_back(items[i]);
            dfs_permutations(items, r, used, current, result);
            current.pop_back();
            used[i] = 0;
        }
    }

    template <class T>
    std::vector<std::vector<T>> permutations(const std::vector<T> &items, int r)
    {
        std::vector<std::vector<T>> result;
        if (r < 0 || r > static_cast<int>(items.size()))
            return result;

        result.reserve(count_permutations(static_cast<int>(items.size()), r));
        std::vector<char> used(items.size(), 0);
        std::vector<T> current;
        current.reserve(r);
        dfs_permutations(items, r, used, current, result);
        return result;
    }

    template <class T>
    std::vector<std::vector<T>> permutations(const std::vector<T> &items)
    {
        return permutations(items, static_cast<int>(items.size()));
    }

    template <class T, class Callback>
    void dfs_permutations_callback(const std::vector<T> &items, int r, std::vector<char> &used,
                                   std::vector<T> &current, Callback &callback)
    {
        if (static_cast<int>(current.size()) == r)
        {
            callback(current);
            return;
        }

        for (int i = 0; i < static_cast<int>(items.size()); ++i)
        {
            if (used[i])
                continue;
            used[i] = 1;
            current.push_back(items[i]);
            dfs_permutations_callback(items, r, used, current, callback);
            current.pop_back();
            used[i] = 0;
        }
    }

    template <class T, class Callback>
    void for_each_permutation(const std::vector<T> &items, int r, Callback &&callback)
    {
        if (r < 0 || r > static_cast<int>(items.size()))
            return;

        auto cb = std::forward<Callback>(callback);
        std::vector<char> used(items.size(), 0);
        std::vector<T> current;
        current.reserve(r);
        dfs_permutations_callback(items, r, used, current, cb);
    }

    template <class T, class Callback>
    void for_each_permutation(const std::vector<T> &items, Callback &&callback)
    {
        for_each_permutation(items, static_cast<int>(items.size()), std::forward<Callback>(callback));
    }

    inline std::vector<int> range_values(int n)
    {
        std::vector<int> values;
        if (n < 0)
            return values;
        values.resize(n);
        for (int i = 0; i < n; ++i)
            values[i] = i;
        return values;
    }

    inline std::vector<std::vector<int>> combinations(int n, int r)
    {
        return combinations(range_values(n), r);
    }

    template <class Callback>
    void for_each_combination(int n, int r, Callback &&callback)
    {
        for_each_combination(range_values(n), r, std::forward<Callback>(callback));
    }

    inline std::vector<std::vector<int>> permutations(int n, int r)
    {
        return permutations(range_values(n), r);
    }

    inline std::vector<std::vector<int>> permutations(int n)
    {
        return permutations(range_values(n), n);
    }

    template <class Callback>
    void for_each_permutation(int n, int r, Callback &&callback)
    {
        for_each_permutation(range_values(n), r, std::forward<Callback>(callback));
    }

    template <class Callback>
    void for_each_permutation(int n, Callback &&callback)
    {
        for_each_permutation(range_values(n), n, std::forward<Callback>(callback));
    }
} // namespace combinatorial_enumeration_lib
