#pragma once

#include <cassert>
#include <type_traits>
#include <vector>

namespace fenwick_tree_lib
{
    namespace internal
    {
#ifndef _MSC_VER
        template <class T>
        using is_signed_int128 =
            typename std::conditional<std::is_same<T, __int128_t>::value ||
                                          std::is_same<T, __int128>::value,
                                      std::true_type,
                                      std::false_type>::type;

        template <class T>
        using make_unsigned_int128 =
            typename std::conditional<std::is_same<T, __int128_t>::value,
                                      __uint128_t,
                                      unsigned __int128>::type;

        template <class T>
        using to_unsigned = typename std::conditional<
            is_signed_int128<T>::value,
            make_unsigned_int128<T>,
            typename std::conditional<std::is_signed<T>::value,
                                      std::make_unsigned<T>,
                                      std::common_type<T>>::type>::type;
#else
        template <class T>
        using to_unsigned = typename std::conditional<std::is_signed<T>::value,
                                                      std::make_unsigned<T>,
                                                      std::common_type<T>>::type;
#endif

        template <class T>
        using to_unsigned_t = typename to_unsigned<T>::type;
    } // namespace internal

    // 0-indexed の Fenwick Tree (Binary Indexed Tree)
    template <class T>
    class fenwick_tree
    {
        using U = internal::to_unsigned_t<T>;

    public:
        fenwick_tree() = default;

        // n 要素を 0 で初期化する
        explicit fenwick_tree(int n)
        {
            assert(0 <= n);
            n_ = n;
            data_.assign(n_, U());
        }

        // 配列 v から O(n) で構築する
        explicit fenwick_tree(const std::vector<T> &v)
        {
            n_ = static_cast<int>(v.size());
            data_.resize(n_);
            for (int i = 0; i < n_; ++i)
                data_[i] = U(v[i]);
            for (int i = 1; i <= n_; ++i)
            {
                int j = i + (i & -i);
                if (j <= n_)
                    data_[j - 1] += data_[i - 1];
            }
        }

        // a[p] += x
        void add(int p, T x)
        {
            assert(0 <= p && p < n_);
            ++p;
            while (p <= n_)
            {
                data_[p - 1] += U(x);
                p += p & -p;
            }
        }

        // 半開区間 [0, r) の和
        T sum(int r) const
        {
            assert(0 <= r && r <= n_);
            return prefix_sum(r);
        }

        // 半開区間 [l, r) の和
        T sum(int l, int r) const
        {
            assert(0 <= l && l <= r && r <= n_);
            return prefix_sum(r) - prefix_sum(l);
        }

        int size() const
        {
            return n_;
        }

    private:
        int n_ = 0;
        std::vector<U> data_;

        U prefix_sum(int r) const
        {
            U s = 0;
            while (r > 0)
            {
                s += data_[r - 1];
                r -= r & -r;
            }
            return s;
        }
    };
} // namespace fenwick_tree_lib
