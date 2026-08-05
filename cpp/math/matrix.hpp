#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace matrix_lib
{
    template <class T>
    class Matrix
    {
    public:
        Matrix() = default;
        // h x w 行列を fill で初期化する
        Matrix(int h, int w, const T &fill = T())
            : h_(h), w_(w), data_(h, std::vector<T>(w, fill)) {}
        // 二次元配列から構築する
        explicit Matrix(std::vector<std::vector<T>> data)
            : h_(static_cast<int>(data.size())),
              w_(data.empty() ? 0 : static_cast<int>(data[0].size())),
              data_(std::move(data)) {}

        int height() const { return h_; }
        int width() const { return w_; }

        std::vector<T> &operator[](int i) { return data_[i]; }
        const std::vector<T> &operator[](int i) const { return data_[i]; }

    private:
        int h_ = 0, w_ = 0;
        std::vector<std::vector<T>> data_;
    };

    // 対角成分が e_aggregate、それ以外が e_combine の行列を作る
    // (combine/aggregate に対する単位行列。mat_pow の初期値に使う)
    template <class T>
    Matrix<T> identity_like(int n, const T &e_combine, const T &e_aggregate)
    {
        Matrix<T> I(n, n, e_combine);
        for (int i = 0; i < n; ++i)
            I[i][i] = e_aggregate;
        return I;
    }

    // C[i][j] = combine_{k}( aggregate(A[i][k], B[k][j]) )
    // combine  : T combine(T a, T b)   畳み込み演算（通常の行列積では +）
    // aggregate: T aggregate(T a, T b) 各項の演算（通常の行列積では ×）
    // e_combine: combine の単位元（畳み込みの初期値。+ なら 0、min なら +INF）
    template <class T, class Combine, class Aggregate>
    Matrix<T> mat_mul(const Matrix<T> &A, const Matrix<T> &B,
                       Combine combine, Aggregate aggregate, const T &e_combine)
    {
        int h = A.height(), mid = A.width(), w = B.width();
        assert(mid == B.height());

        Matrix<T> C(h, w, e_combine);
        for (int i = 0; i < h; ++i)
            for (int k = 0; k < mid; ++k)
                for (int j = 0; j < w; ++j)
                    C[i][j] = combine(C[i][j], aggregate(A[i][k], B[k][j]));
        return C;
    }

    // オーバーフローせずに a + b を計算する（a か b が cap 以上なら cap を返す）
    // min-plus 行列で「到達不可(+INF)」同士を + で畳み込むと INF+INF が
    // long long の範囲を超えて桁あふれすることがあるため、aggregate に + を渡す際は
    // これで包んで capped_add(a, b, INFLL) の形で使うこと。
    template <class T>
    T capped_add(const T &a, const T &b, const T &cap)
    {
        if (a >= cap || b >= cap)
            return cap;
        return a + b;
    }

    // A を n 乗した行列を返す（二分累乗、正方行列を想定）
    // e_combine  : combine の単位元（mat_mul と同じ）
    // e_aggregate: aggregate の単位元（単位行列の対角成分。× なら 1、+ なら 0）
    template <class T, class Combine, class Aggregate>
    Matrix<T> mat_pow(Matrix<T> A, long long n,
                       Combine combine, Aggregate aggregate,
                       const T &e_combine, const T &e_aggregate)
    {
        assert(A.height() == A.width());
        assert(n >= 0);

        Matrix<T> R = identity_like(A.height(), e_combine, e_aggregate);
        while (n > 0)
        {
            if (n & 1)
                R = mat_mul(R, A, combine, aggregate, e_combine);
            if (n > 1)
                A = mat_mul(A, A, combine, aggregate, e_combine);
            n >>= 1;
        }
        return R;
    }
} // namespace matrix_lib
