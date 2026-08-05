// ===== BEGIN MODINT COPY BLOCK =====
// Copy and paste from this line to the END marker below.
// clang-format off
#ifndef MODINT_LIBRARY_MODINT_HPP
#define MODINT_LIBRARY_MODINT_HPP 1

#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace modint_lib {
namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T> using is_modint = std::is_base_of<modint_base, T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;

template <class T>
using is_signed_int_t = std::enable_if_t<std::is_integral<T>::value &&
                                         std::is_signed<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<std::is_integral<T>::value &&
                                           std::is_unsigned<T>::value>;

constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int um = (unsigned int)m;
    unsigned long long r = 1;
    unsigned long long y = (unsigned long long)safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % um;
        y = (y * y) % um;
        n >>= 1;
    }
    return (long long)r;
}

constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if ((n & 1) == 0) return false;

    long long d = n - 1;
    while ((d & 1) == 0) d >>= 1;
    constexpr long long bases[3] = {2, 7, 61};

    for (long long a : bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = (y * y) % n;
            t <<= 1;
        }
        if (y != n - 1 && (t & 1) == 0) return false;
    }
    return true;
}

template <int n> constexpr bool is_prime = is_prime_constexpr(n);

constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;
        std::swap(s, t);
        std::swap(m0, m1);
    }

    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

struct barrett {
    unsigned int m_;
    unsigned long long im_;

    explicit barrett(unsigned int m) : m_(m), im_((unsigned long long)(-1) / m + 1) {}

    unsigned int umod() const { return m_; }

    unsigned int mul(unsigned int a, unsigned int b) const {
        unsigned long long z = (unsigned long long)a * b;
#ifdef _MSC_VER
        unsigned long long x;
        _umul128(z, im_, &x);
#else
        unsigned long long x = (unsigned long long)(((__uint128_t)z * im_) >> 64);
#endif
        unsigned long long y = x * m_;
        return (unsigned int)(z - y + (z < y ? m_ : 0));
    }
};

} // namespace internal

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
    using Modint = static_modint;

    static constexpr int mod() { return m; }

    static Modint raw(int v) {
        Modint x;
        x.v_ = (unsigned int)v;
        return x;
    }

    static_modint() : v_(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    static_modint(T v) {
        long long x = (long long)(v % (long long)umod());
        if (x < 0) x += umod();
        v_ = (unsigned int)x;
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    static_modint(T v) {
        v_ = (unsigned int)(v % umod());
    }

    int val() const { return (int)v_; }

    Modint& operator++() {
        ++v_;
        if (v_ == umod()) v_ = 0;
        return *this;
    }

    Modint& operator--() {
        if (v_ == 0) v_ = umod();
        --v_;
        return *this;
    }

    Modint operator++(int) {
        Modint r = *this;
        ++*this;
        return r;
    }

    Modint operator--(int) {
        Modint r = *this;
        --*this;
        return r;
    }

    Modint& operator+=(const Modint& rhs) {
        v_ += rhs.v_;
        if (v_ >= umod()) v_ -= umod();
        return *this;
    }

    Modint& operator-=(const Modint& rhs) {
        v_ -= rhs.v_;
        if (v_ >= umod()) v_ += umod();
        return *this;
    }

    Modint& operator*=(const Modint& rhs) {
        unsigned long long z = (unsigned long long)v_ * rhs.v_;
        v_ = (unsigned int)(z % umod());
        return *this;
    }

    Modint& operator/=(const Modint& rhs) { return *this *= rhs.inv(); }

    Modint operator+() const { return *this; }
    Modint operator-() const { return Modint() - *this; }

    Modint pow(long long n) const {
        assert(0 <= n);
        Modint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    Modint inv() const {
        if (prime_) {
            assert(v_ != 0);
            return pow((long long)umod() - 2);
        }
        auto eg = internal::inv_gcd((long long)v_, m);
        assert(eg.first == 1);
        return Modint((int)eg.second);
    }

    friend Modint operator+(Modint lhs, const Modint& rhs) { return lhs += rhs; }
    friend Modint operator-(Modint lhs, const Modint& rhs) { return lhs -= rhs; }
    friend Modint operator*(Modint lhs, const Modint& rhs) { return lhs *= rhs; }
    friend Modint operator/(Modint lhs, const Modint& rhs) { return lhs /= rhs; }

    friend bool operator==(const Modint& lhs, const Modint& rhs) {
        return lhs.v_ == rhs.v_;
    }

    friend bool operator!=(const Modint& lhs, const Modint& rhs) {
        return lhs.v_ != rhs.v_;
    }

  private:
    unsigned int v_;

    static constexpr unsigned int umod() { return (unsigned int)m; }
    static constexpr bool prime_ = internal::is_prime<m>;
};

template <int id>
struct dynamic_modint : internal::modint_base {
    using Modint = dynamic_modint;

    static int mod() { return (int)bt_.umod(); }

    static void set_mod(int m) {
        assert(1 <= m);
        bt_ = internal::barrett((unsigned int)m);
    }

    static Modint raw(int v) {
        Modint x;
        x.v_ = (unsigned int)v;
        return x;
    }

    dynamic_modint() : v_(0) {}

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        long long x = (long long)(v % (long long)mod());
        if (x < 0) x += mod();
        v_ = (unsigned int)x;
    }

    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        v_ = (unsigned int)(v % (unsigned int)mod());
    }

    int val() const { return (int)v_; }

    Modint& operator++() {
        ++v_;
        if (v_ == umod()) v_ = 0;
        return *this;
    }

    Modint& operator--() {
        if (v_ == 0) v_ = umod();
        --v_;
        return *this;
    }

    Modint operator++(int) {
        Modint r = *this;
        ++*this;
        return r;
    }

    Modint operator--(int) {
        Modint r = *this;
        --*this;
        return r;
    }

    Modint& operator+=(const Modint& rhs) {
        v_ += rhs.v_;
        if (v_ >= umod()) v_ -= umod();
        return *this;
    }

    Modint& operator-=(const Modint& rhs) {
        v_ += umod() - rhs.v_;
        if (v_ >= umod()) v_ -= umod();
        return *this;
    }

    Modint& operator*=(const Modint& rhs) {
        v_ = bt_.mul(v_, rhs.v_);
        return *this;
    }

    Modint& operator/=(const Modint& rhs) { return *this *= rhs.inv(); }

    Modint operator+() const { return *this; }
    Modint operator-() const { return Modint() - *this; }

    Modint pow(long long n) const {
        assert(0 <= n);
        Modint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    Modint inv() const {
        auto eg = internal::inv_gcd((long long)v_, mod());
        assert(eg.first == 1);
        return Modint((int)eg.second);
    }

    friend Modint operator+(Modint lhs, const Modint& rhs) { return lhs += rhs; }
    friend Modint operator-(Modint lhs, const Modint& rhs) { return lhs -= rhs; }
    friend Modint operator*(Modint lhs, const Modint& rhs) { return lhs *= rhs; }
    friend Modint operator/(Modint lhs, const Modint& rhs) { return lhs /= rhs; }

    friend bool operator==(const Modint& lhs, const Modint& rhs) {
        return lhs.v_ == rhs.v_;
    }

    friend bool operator!=(const Modint& lhs, const Modint& rhs) {
        return lhs.v_ != rhs.v_;
    }

  private:
    unsigned int v_;
    static internal::barrett bt_;

    static unsigned int umod() { return bt_.umod(); }
};

template <int id>
internal::barrett dynamic_modint<id>::bt_(998244353);

using Modint9 = static_modint<998244353>;
using Modint1 = static_modint<1000000007>;
using Modint = dynamic_modint<-1>;

template <class Mint, internal::is_modint_t<Mint>* = nullptr>
struct Binomial {
    std::vector<Mint> fac;
    std::vector<Mint> ifac;

    Binomial() : fac(1, Mint(1)), ifac(1, Mint(1)) {}
    explicit Binomial(int n) : Binomial() { ensure(n); }
    explicit Binomial(long long n) : Binomial() { ensure(n); }

    void ensure(int n) {
        assert(n >= 0);
        int cur = (int)fac.size() - 1;
        if (n <= cur) return;

        fac.resize(n + 1);
        ifac.resize(n + 1);
        for (int i = cur + 1; i <= n; ++i) {
            fac[i] = fac[i - 1] * Mint(i);
        }
        ifac[n] = fac[n].inv();
        for (int i = n; i > cur; --i) {
            ifac[i - 1] = ifac[i] * Mint(i);
        }
    }

    void ensure(long long n) {
        if (n < 0) return;
        if (n > (long long)std::numeric_limits<int>::max()) {
            assert(false && "Binomial::ensure: n must fit in int");
            return;
        }
        ensure((int)n);
    }

    Mint C(int n, int k) {
        if (n < 0 || k < 0 || k > n) return Mint(0);
        ensure(n);
        return fac[n] * ifac[k] * ifac[n - k];
    }

    Mint C(long long n, long long k) {
        if (n < 0 || k < 0 || k > n) return Mint(0);
        if (n > (long long)std::numeric_limits<int>::max() ||
            k > (long long)std::numeric_limits<int>::max()) {
            assert(false && "Binomial::C: n and k must fit in int");
            return Mint(0);
        }
        return C((int)n, (int)k);
    }

    Mint P(int n, int k) {
        if (n < 0 || k < 0 || k > n) return Mint(0);
        ensure(n);
        return fac[n] * ifac[n - k];
    }

    Mint P(long long n, long long k) {
        if (n < 0 || k < 0 || k > n) return Mint(0);
        if (n > (long long)std::numeric_limits<int>::max() ||
            k > (long long)std::numeric_limits<int>::max()) {
            assert(false && "Binomial::P: n and k must fit in int");
            return Mint(0);
        }
        return P((int)n, (int)k);
    }

    Mint H(int n, int k) {
        if (n < 0 || k < 0) return Mint(0);
        if (n == 0) return Mint(k == 0);
        return C(n + k - 1, k);
    }

    Mint H(long long n, long long k) {
        if (n < 0 || k < 0) return Mint(0);
        if (n == 0) return Mint(k == 0);
        if (n > std::numeric_limits<long long>::max() - k + 1) {
            assert(false && "Binomial::H: n + k - 1 overflowed long long");
            return Mint(0);
        }
        return C(n + k - 1, k);
    }
};

namespace internal {
template <class T> using is_static_modint = std::is_base_of<static_modint_base, T>;
template <class T> using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class> struct is_dynamic_modint : std::false_type {};
template <int id> struct is_dynamic_modint<dynamic_modint<id>> : std::true_type {};

template <class T> using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;
} // namespace internal

} // namespace modint_lib

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
using StaticModint = modint_lib::static_modint<m>;

template <int id>
using DynamicModint = modint_lib::dynamic_modint<id>;

using Modint9 = modint_lib::Modint9;
using Modint1 = modint_lib::Modint1;
using Modint = modint_lib::Modint;

#endif // MODINT_LIBRARY_MODINT_HPP
// clang-format on
// ===== END MODINT COPY BLOCK =====
