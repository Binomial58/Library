#pragma once

#include <cassert>
#include <functional>
#include <type_traits>
#include <vector>

namespace lazy_segtree_lib
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
    // mapping    : S mapping(F f, S x)
    // composition: F composition(F f, F g)
    // id         : F id()
    template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
    class lazy_segtree
    {
        static_assert(std::is_convertible_v<decltype(op), std::function<S(S, S)>>,
                      "op must work as S(S, S)");
        static_assert(std::is_convertible_v<decltype(e), std::function<S()>>,
                      "e must work as S()");
        static_assert(std::is_convertible_v<decltype(mapping), std::function<S(F, S)>>,
                      "mapping must work as S(F, S)");
        static_assert(std::is_convertible_v<decltype(composition), std::function<F(F, F)>>,
                      "composition must work as F(F, F)");
        static_assert(std::is_convertible_v<decltype(id), std::function<F()>>,
                      "id must work as F()");
#else
    // op: S op(S a, S b)
    // e : S e()
    // mapping    : S mapping(F f, S x)
    // composition: F composition(F f, F g)
    // id         : F id()
    template <class S,
              S (*op)(S, S),
              S (*e)(),
              class F,
              S (*mapping)(F, S),
              F (*composition)(F, F),
              F (*id)()>
    class lazy_segtree
    {
#endif
    public:
        // 長さ 0 の遅延セグ木を作る
        lazy_segtree() : lazy_segtree(0) {}

        // n 要素（すべて e()）で初期化する
        explicit lazy_segtree(int n) : lazy_segtree(std::vector<S>(n, e())) {}

        // 配列 v から遅延セグ木を構築する
        explicit lazy_segtree(const std::vector<S> &v) : n_(static_cast<int>(v.size()))
        {
            size_ = static_cast<int>(internal::bit_ceil(static_cast<unsigned int>(n_)));
            log_ = internal::countr_zero(static_cast<unsigned int>(size_));
            data_.assign(2 * size_, e());
            lazy_.assign(size_, id());
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
            for (int i = log_; i >= 1; --i)
                push(p >> i);
            data_[p] = x;
            for (int i = 1; i <= log_; ++i)
                update(p >> i);
        }

        // a[p] を返す
        S get(int p)
        {
            assert(0 <= p && p < n_);
            p += size_;
            for (int i = log_; i >= 1; --i)
                push(p >> i);
            return data_[p];
        }

        // 半開区間 [l, r) の積を返す
        S prod(int l, int r)
        {
            assert(0 <= l && l <= r && r <= n_);
            if (l == r)
                return e();

            l += size_;
            r += size_;

            for (int i = log_; i >= 1; --i)
            {
                if (((l >> i) << i) != l)
                    push(l >> i);
                if (((r >> i) << i) != r)
                    push((r - 1) >> i);
            }

            S sml = e();
            S smr = e();
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

        // a[p] に作用 f を適用する
        void apply(int p, F f)
        {
            assert(0 <= p && p < n_);
            p += size_;
            for (int i = log_; i >= 1; --i)
                push(p >> i);
            data_[p] = mapping(f, data_[p]);
            for (int i = 1; i <= log_; ++i)
                update(p >> i);
        }

        // 半開区間 [l, r) に作用 f を適用する
        void apply(int l, int r, F f)
        {
            assert(0 <= l && l <= r && r <= n_);
            if (l == r)
                return;

            l += size_;
            r += size_;

            for (int i = log_; i >= 1; --i)
            {
                if (((l >> i) << i) != l)
                    push(l >> i);
                if (((r >> i) << i) != r)
                    push((r - 1) >> i);
            }

            int l2 = l;
            int r2 = r;
            while (l < r)
            {
                if (l & 1)
                    all_apply(l++, f);
                if (r & 1)
                    all_apply(--r, f);
                l >>= 1;
                r >>= 1;
            }
            l = l2;
            r = r2;

            for (int i = 1; i <= log_; ++i)
            {
                if (((l >> i) << i) != l)
                    update(l >> i);
                if (((r >> i) << i) != r)
                    update((r - 1) >> i);
            }
        }

        // 述語 g を使って max_right を求める（関数ポインタ版）
        template <bool (*g)(S)>
        int max_right(int l)
        {
            return max_right(l, [](S x) { return g(x); });
        }

        // g(prod(l, r)) が true となる最大の r を返す
        template <class G>
        int max_right(int l, G g)
        {
            assert(0 <= l && l <= n_);
            assert(g(e()));
            if (l == n_)
                return n_;

            l += size_;
            for (int i = log_; i >= 1; --i)
                push(l >> i);

            S sm = e();
            do
            {
                while ((l % 2) == 0)
                    l >>= 1;
                if (!g(op(sm, data_[l])))
                {
                    while (l < size_)
                    {
                        push(l);
                        l <<= 1;
                        if (g(op(sm, data_[l])))
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

        // 述語 g を使って min_left を求める（関数ポインタ版）
        template <bool (*g)(S)>
        int min_left(int r)
        {
            return min_left(r, [](S x) { return g(x); });
        }

        // g(prod(l, r)) が true となる最小の l を返す
        template <class G>
        int min_left(int r, G g)
        {
            assert(0 <= r && r <= n_);
            assert(g(e()));
            if (r == 0)
                return 0;

            r += size_;
            for (int i = log_; i >= 1; --i)
                push((r - 1) >> i);

            S sm = e();
            do
            {
                --r;
                while (r > 1 && (r % 2))
                    r >>= 1;
                if (!g(op(data_[r], sm)))
                {
                    while (r < size_)
                    {
                        push(r);
                        r = (r << 1) + 1;
                        if (g(op(data_[r], sm)))
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
        std::vector<F> lazy_;

        // k 番ノードを子から再計算する
        void update(int k)
        {
            data_[k] = op(data_[2 * k], data_[2 * k + 1]);
        }

        // ノード k 全体に作用 f を適用する
        void all_apply(int k, F f)
        {
            data_[k] = mapping(f, data_[k]);
            if (k < size_)
                lazy_[k] = composition(f, lazy_[k]);
        }

        // 保留中の遅延作用を子に伝播する
        void push(int k)
        {
            all_apply(2 * k, lazy_[k]);
            all_apply(2 * k + 1, lazy_[k]);
            lazy_[k] = id();
        }
    };
} // namespace lazy_segtree_lib

// 例: 区間加算・区間和
// struct S { long long sum; int size; };
// struct F { long long add; };
// S op(S a, S b) { return {a.sum + b.sum, a.size + b.size}; }
// S e() { return {0LL, 0}; }
// S mapping(F f, S x) { return {x.sum + f.add * x.size, x.size}; }
// F composition(F f, F g) { return {f.add + g.add}; }
// F id() { return {0LL}; }
