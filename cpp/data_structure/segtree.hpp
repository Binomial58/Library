#pragma once

#include <cassert>
#include <functional>
#include <type_traits>
#include <vector>

namespace segtree_lib
{
    namespace internal
    {
        // n 以上の最小の 2 べきを返す
        inline unsigned int bit_ceil(unsigned int n)
        {
            unsigned int x = 1;
            while (x < n)
                x <<= 1;
            return x;
        }

        // 末尾の 0 の個数を返す（n > 0 を想定）
        inline int countr_zero(unsigned int n)
        {
            return __builtin_ctz(n);
        }
    } // namespace internal

#if __cplusplus >= 201703L
    // op: S op(S a, S b)
    // e : S e()
    template <class S, auto op, auto e>
    class segtree
    {
        static_assert(std::is_convertible_v<decltype(op), std::function<S(S, S)>>,
                      "op must work as S(S, S)");
        static_assert(std::is_convertible_v<decltype(e), std::function<S()>>,
                      "e must work as S()");
#else
    // op: S op(S a, S b)
    // e : S e()
    template <class S, S (*op)(S, S), S (*e)()>
    class segtree
    {
#endif
    public:
        // 長さ 0 のセグ木を作る
        segtree() : segtree(0) {}
        // n 要素（すべて e()）で初期化する
        explicit segtree(int n) : segtree(std::vector<S>(n, e())) {}
        // 配列 v からセグ木を構築する
        explicit segtree(const std::vector<S> &v) : n_(static_cast<int>(v.size()))
        {
            size_ = static_cast<int>(internal::bit_ceil(static_cast<unsigned int>(n_)));
            log_ = internal::countr_zero(static_cast<unsigned int>(size_));
            data_.assign(2 * size_, e());
            for (int i = 0; i < n_; ++i)
                data_[size_ + i] = v[i];
            for (int i = size_ - 1; i >= 1; --i)
                update(i);
        }

        // a[p] = x に更新する
        void set(int p, S x)
        {
            assert(0 <= p && p < n_);
            p += size_;
            data_[p] = x;
            for (int i = 1; i <= log_; ++i)
                update(p >> i);
        }

        // a[p] を返す
        S get(int p) const
        {
            assert(0 <= p && p < n_);
            return data_[p + size_];
        }

        // 全要素を vector として取り出す（デバッグ用）
        std::vector<S> to_vector() const
        {
            std::vector<S> res(n_);
            for (int i = 0; i < n_; ++i)
                res[i] = get(i);
            return res;
        }

        // 半開区間 [l, r) の積を返す
        S prod(int l, int r) const
        {
            assert(0 <= l && l <= r && r <= n_);
            S sml = e();
            S smr = e();
            l += size_;
            r += size_;

            while (l < r)
            {
                if (l & 1)
                    sml = op(sml, data_[l++]);
                if (r & 1)
                    smr = op(data_[--r], smr);
                l >>= 1;
                r >>= 1;
            }
            return op(sml, smr);
        }

        // 配列全体の積を返す
        S all_prod() const
        {
            return data_[1];
        }

        // 述語 f を使って max_right を求める（関数ポインタ版）
        template <bool (*f)(S)>
        int max_right(int l) const
        {
            return max_right(l, [](S x) { return f(x); });
        }

        // f(prod(l, r)) が true となる最大の r を返す
        template <class F>
        int max_right(int l, F f) const
        {
            assert(0 <= l && l <= n_);
            assert(f(e()));
            if (l == n_)
                return n_;

            l += size_;
            S sm = e();
            do
            {
                while ((l % 2) == 0)
                    l >>= 1;
                if (!f(op(sm, data_[l])))
                {
                    while (l < size_)
                    {
                        l <<= 1;
                        if (f(op(sm, data_[l])))
                        {
                            sm = op(sm, data_[l]);
                            ++l;
                        }
                    }
                    return l - size_;
                }
                sm = op(sm, data_[l]);
                ++l;
            } while ((l & -l) != l);
            return n_;
        }

        // 述語 f を使って min_left を求める（関数ポインタ版）
        template <bool (*f)(S)>
        int min_left(int r) const
        {
            return min_left(r, [](S x) { return f(x); });
        }

        // f(prod(l, r)) が true となる最小の l を返す
        template <class F>
        int min_left(int r, F f) const
        {
            assert(0 <= r && r <= n_);
            assert(f(e()));
            if (r == 0)
                return 0;

            r += size_;
            S sm = e();
            do
            {
                --r;
                while (r > 1 && (r % 2))
                    r >>= 1;
                if (!f(op(data_[r], sm)))
                {
                    while (r < size_)
                    {
                        r = (r << 1) + 1;
                        if (f(op(data_[r], sm)))
                        {
                            sm = op(data_[r], sm);
                            --r;
                        }
                    }
                    return r + 1 - size_;
                }
                sm = op(data_[r], sm);
            } while ((r & -r) != r);
            return 0;
        }

    private:
        int n_ = 0;
        int size_ = 1;
        int log_ = 0;
        std::vector<S> data_;

        // k 番ノードを子から再計算する
        void update(int k)
        {
            data_[k] = op(data_[2 * k], data_[2 * k + 1]);
        }
    };
} // namespace segtree_lib

// 例: 区間最大値クエリ用の op/e
// using ll = long long;
// ll op(ll a, ll b) { return (a < b ? b : a); }
// ll e() { return -(1LL << 60); }
