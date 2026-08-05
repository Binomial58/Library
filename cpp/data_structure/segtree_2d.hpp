#pragma once

#include <cassert>
#include <vector>

namespace segtree_2d_lib
{
    // Intended for commutative monoids such as sum, min, max, and gcd.
    // Based on Nyaan's Library 2d-segment-tree.hpp (CC0-1.0).
    // https://nyaannyaan.github.io/library/data-structure-2d/2d-segment-tree.hpp
    template <class T, class F>
    class segtree_2d
    {
    public:
        // h x w の二次元配列を単位元 e で初期化する
        segtree_2d(int h, int w, F op, const T &e)
            : height_(h), width_(w), op_(op), e_(e)
        {
            assert(0 <= h && 0 <= w);
            init_storage();
        }

        // 二次元配列 v から構築する
        segtree_2d(const std::vector<std::vector<T>> &v, F op, const T &e)
            : height_(static_cast<int>(v.size())),
              width_(v.empty() ? 0 : static_cast<int>(v[0].size())),
              op_(op),
              e_(e)
        {
            for (const auto &row : v)
                assert(static_cast<int>(row.size()) == width_);

            init_storage();
            for (int h = 0; h < height_; ++h)
            {
                for (int w = 0; w < width_; ++w)
                    set(h, w, v[h][w]);
            }
            build();
        }

        // build() の前にだけ使う。一点に値をセットする。
        void set(int h, int w, const T &x)
        {
            assert(0 <= h && h < height_);
            assert(0 <= w && w < width_);
            data_[id(h + size_h_, w + size_w_)] = x;
        }

        // set() で入れた値から全体を構築する
        void build()
        {
            for (int w = size_w_; w < 2 * size_w_; ++w)
            {
                for (int h = size_h_ - 1; h >= 1; --h)
                    data_[id(h, w)] = op_(data_[id(2 * h, w)], data_[id(2 * h + 1, w)]);
            }

            for (int h = 0; h < 2 * size_h_; ++h)
            {
                for (int w = size_w_ - 1; w >= 1; --w)
                    data_[id(h, w)] = op_(data_[id(h, 2 * w)], data_[id(h, 2 * w + 1)]);
            }
        }

        // a[h][w] を返す
        T get(int h, int w) const
        {
            assert(0 <= h && h < height_);
            assert(0 <= w && w < width_);
            return data_[id(h + size_h_, w + size_w_)];
        }

        T operator()(int h, int w) const
        {
            return get(h, w);
        }

        // a[h][w] = x に更新する
        void update(int h, int w, const T &x)
        {
            assert(0 <= h && h < height_);
            assert(0 <= w && w < width_);

            h += size_h_;
            w += size_w_;
            data_[id(h, w)] = x;

            for (int i = h >> 1; i >= 1; i >>= 1)
                data_[id(i, w)] = op_(data_[id(2 * i, w)], data_[id(2 * i + 1, w)]);

            for (; h >= 1; h >>= 1)
            {
                for (int j = w >> 1; j >= 1; j >>= 1)
                    data_[id(h, j)] = op_(data_[id(h, 2 * j)], data_[id(h, 2 * j + 1)]);
            }
        }

        // 半開矩形 [h1, h2) x [w1, w2) の積を返す
        T query(int h1, int w1, int h2, int w2) const
        {
            assert(0 <= h1 && h1 <= h2 && h2 <= height_);
            assert(0 <= w1 && w1 <= w2 && w2 <= width_);
            if (h1 == h2 || w1 == w2)
                return e_;

            T upper = e_;
            T lower = e_;
            h1 += size_h_;
            h2 += size_h_;
            w1 += size_w_;
            w2 += size_w_;

            while (h1 < h2)
            {
                if (h1 & 1)
                    upper = op_(upper, inner_query(h1++, w1, w2));
                if (h2 & 1)
                    lower = op_(inner_query(--h2, w1, w2), lower);
                h1 >>= 1;
                h2 >>= 1;
            }
            return op_(upper, lower);
        }

        T prod(int h1, int w1, int h2, int w2) const
        {
            return query(h1, w1, h2, w2);
        }

        int height() const
        {
            return height_;
        }

        int width() const
        {
            return width_;
        }

    private:
        int height_ = 0;
        int width_ = 0;
        int size_h_ = 1;
        int size_w_ = 1;
        F op_;
        T e_;
        std::vector<T> data_;

        int id(int h, int w) const
        {
            return h * 2 * size_w_ + w;
        }

        void init_storage()
        {
            size_h_ = 1;
            size_w_ = 1;
            while (size_h_ < height_)
                size_h_ <<= 1;
            while (size_w_ < width_)
                size_w_ <<= 1;
            data_.assign(4 * size_h_ * size_w_, e_);
        }

        T inner_query(int h, int w1, int w2) const
        {
            T left = e_;
            T right = e_;
            while (w1 < w2)
            {
                if (w1 & 1)
                    left = op_(left, data_[id(h, w1++)]);
                if (w2 & 1)
                    right = op_(data_[id(h, --w2)], right);
                w1 >>= 1;
                w2 >>= 1;
            }
            return op_(left, right);
        }
    };
} // namespace segtree_2d_lib
