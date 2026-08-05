// ===== BEGIN CONVOLUTION COPY BLOCK =====
// Copy and paste from this line to the END marker below.
// Requires Modint.hpp's COPY BLOCK to be pasted somewhere above this block.
// clang-format off
#ifndef CONVOLUTION_LIBRARY_CONVOLUTION_HPP
#define CONVOLUTION_LIBRARY_CONVOLUTION_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

#ifndef MODINT_LIBRARY_MODINT_HPP
#include "../math/Modint.hpp"
#endif

namespace convolution_lib {
namespace internal {

inline int primitive_root(int mod) {
    assert(mod >= 2);
    if (mod == 2) return 1;
    if (mod == 998244353) return 3;
    if (mod == 167772161) return 3;
    if (mod == 469762049) return 3;
    if (mod == 754974721) return 11;

    int divs[20] = {};
    int cnt = 0;
    int x = mod - 1;
    for (int p = 2; (long long)p * p <= x; ++p) {
        if (x % p != 0) continue;
        divs[cnt++] = p;
        while (x % p == 0) x /= p;
    }
    if (x > 1) divs[cnt++] = x;

    for (int g = 2;; ++g) {
        bool ok = true;
        for (int i = 0; i < cnt; ++i) {
            if (modint_lib::internal::pow_mod_constexpr(
                    g, (mod - 1) / divs[i], mod) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}

template <class Mint>
bool can_ntt(int size) {
    static_assert(modint_lib::internal::is_modint<Mint>::value,
                  "convolution requires a modint type from Modint.hpp");
    const int mod = Mint::mod();
    return size > 0 && (size & (size - 1)) == 0 &&
           modint_lib::internal::is_prime_constexpr(mod) &&
           (mod - 1) % size == 0;
}

template <class Mint>
void ntt(std::vector<Mint>& a, bool inverse) {
    const int n = static_cast<int>(a.size());
    assert(can_ntt<Mint>(n));

    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }

    const int mod = Mint::mod();
    const Mint root = Mint(primitive_root(mod));
    for (int len = 2; len <= n; len <<= 1) {
        Mint wlen = root.pow((mod - 1) / len);
        if (inverse) wlen = wlen.inv();
        for (int i = 0; i < n; i += len) {
            Mint w = 1;
            for (int j = 0; j < len / 2; ++j) {
                Mint u = a[i + j];
                Mint v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
        if (len == n) break;
    }

    if (inverse) {
        Mint inv_n = Mint(n).inv();
        for (Mint& x : a) x *= inv_n;
    }
}

template <class Mint>
std::vector<Mint> convolution_ntt(std::vector<Mint> a,
                                  std::vector<Mint> b) {
    const int result_size = static_cast<int>(a.size() + b.size() - 1);
    int ntt_size = 1;
    while (ntt_size < result_size) ntt_size <<= 1;
    assert(can_ntt<Mint>(ntt_size));

    a.resize(ntt_size);
    b.resize(ntt_size);
    ntt(a, false);
    ntt(b, false);
    for (int i = 0; i < ntt_size; ++i) a[i] *= b[i];
    ntt(a, true);
    a.resize(result_size);
    return a;
}

inline bool crt_reconstruction_is_unique(int min_size, int target_mod) {
    constexpr unsigned long long mod0 = 754974721;
    constexpr unsigned long long mod1 = 167772161;
    constexpr unsigned long long mod2 = 469762049;
    const unsigned __int128 crt_mod =
        static_cast<unsigned __int128>(mod0) * mod1 * mod2;
    const unsigned __int128 coefficient_bound =
        static_cast<unsigned __int128>(min_size) * (target_mod - 1) *
        (target_mod - 1);
    return coefficient_bound < crt_mod;
}

template <class Mint>
std::vector<Mint> convolution_crt(const std::vector<Mint>& a,
                                  const std::vector<Mint>& b) {
    constexpr int mod0 = 754974721;
    constexpr int mod1 = 167772161;
    constexpr int mod2 = 469762049;
    using Mint0 = StaticModint<mod0>;
    using Mint1 = StaticModint<mod1>;
    using Mint2 = StaticModint<mod2>;

    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<Mint0> a0(n), b0(m);
    std::vector<Mint1> a1(n), b1(m);
    std::vector<Mint2> a2(n), b2(m);
    for (int i = 0; i < n; ++i) {
        a0[i] = a[i].val();
        a1[i] = a[i].val();
        a2[i] = a[i].val();
    }
    for (int i = 0; i < m; ++i) {
        b0[i] = b[i].val();
        b1[i] = b[i].val();
        b2[i] = b[i].val();
    }

    std::vector<Mint0> c0 = convolution_ntt(std::move(a0), std::move(b0));
    std::vector<Mint1> c1 = convolution_ntt(std::move(a1), std::move(b1));
    std::vector<Mint2> c2 = convolution_ntt(std::move(a2), std::move(b2));

    const Mint1 inv_mod0_mod1 = Mint1(mod0).inv();
    const Mint2 inv_mod01_mod2 =
        (Mint2(mod0) * Mint2(mod1)).inv();
    const Mint target_mod0 = Mint(mod0);
    const Mint target_mod01 = target_mod0 * Mint(mod1);

    std::vector<Mint> result(c0.size());
    for (int i = 0; i < static_cast<int>(result.size()); ++i) {
        const unsigned int x0 = c0[i].val();
        const unsigned int x1 =
            ((c1[i] - Mint1(x0)) * inv_mod0_mod1).val();
        const unsigned int x2 =
            ((c2[i] - Mint2(x0) - Mint2(mod0) * Mint2(x1)) *
             inv_mod01_mod2)
                .val();
        result[i] = Mint(x0) + target_mod0 * Mint(x1) +
                    target_mod01 * Mint(x2);
    }
    return result;
}

} // namespace internal

template <class Mint>
std::vector<Mint> convolution_naive(const std::vector<Mint>& a,
                                    const std::vector<Mint>& b) {
    static_assert(modint_lib::internal::is_modint<Mint>::value,
                  "convolution requires a modint type from Modint.hpp");
    if (a.empty() || b.empty()) return {};
    std::vector<Mint> result(a.size() + b.size() - 1);
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        for (int j = 0; j < static_cast<int>(b.size()); ++j) {
            result[i + j] += a[i] * b[j];
        }
    }
    return result;
}

template <class Mint>
std::vector<Mint> convolution(const std::vector<Mint>& a,
                              const std::vector<Mint>& b) {
    static_assert(modint_lib::internal::is_modint<Mint>::value,
                  "convolution requires a modint type from Modint.hpp");
    if (a.empty() || b.empty()) return {};
    if (std::min(a.size(), b.size()) <= 60) {
        return convolution_naive(a, b);
    }

    const int result_size = static_cast<int>(a.size() + b.size() - 1);
    int ntt_size = 1;
    while (ntt_size < result_size) ntt_size <<= 1;
    if (internal::can_ntt<Mint>(ntt_size)) {
        return internal::convolution_ntt(std::vector<Mint>(a),
                                         std::vector<Mint>(b));
    }
    if (internal::can_ntt<StaticModint<754974721>>(ntt_size) &&
        internal::can_ntt<StaticModint<167772161>>(ntt_size) &&
        internal::can_ntt<StaticModint<469762049>>(ntt_size) &&
        internal::crt_reconstruction_is_unique(
            static_cast<int>(std::min(a.size(), b.size())), Mint::mod())) {
        return internal::convolution_crt(a, b);
    }
    return convolution_naive(a, b);
}

} // namespace convolution_lib

#endif // CONVOLUTION_LIBRARY_CONVOLUTION_HPP
// clang-format on
// ===== END CONVOLUTION COPY BLOCK =====
