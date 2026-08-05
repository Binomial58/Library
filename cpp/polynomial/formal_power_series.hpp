// ===== BEGIN FORMAL POWER SERIES COPY BLOCK =====
// Copy and paste from this line to the END marker below.
// Requires Modint.hpp's and Convolution's COPY BLOCKs pasted somewhere above this block.
// clang-format off
#ifndef FORMAL_POWER_SERIES_LIBRARY_HPP
#define FORMAL_POWER_SERIES_LIBRARY_HPP 1

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>

#ifndef MODINT_LIBRARY_MODINT_HPP
#include "../math/Modint.hpp"
#endif
#ifndef CONVOLUTION_LIBRARY_CONVOLUTION_HPP
#include "convolution.hpp"
#endif

namespace formal_power_series_lib {

template <class Mint>
class FormalPowerSeries : public std::vector<Mint> {
    static_assert(modint_lib::internal::is_modint<Mint>::value,
                  "FormalPowerSeries requires a modint type from Modint.hpp");

    using Base = std::vector<Mint>;

  public:
    using Base::Base;

    FormalPowerSeries() = default;
    FormalPowerSeries(const Base& v) : Base(v) {}
    FormalPowerSeries(Base&& v) : Base(std::move(v)) {}
    FormalPowerSeries(std::initializer_list<Mint> init) : Base(init) {}

    int count_nonzero(int n = -1) const {
        const int limit =
            n < 0 ? static_cast<int>(this->size())
                  : std::min<int>(n, this->size());
        int count = 0;
        for (int i = 0; i < limit; ++i) count += ((*this)[i] != Mint(0));
        return count;
    }

    FormalPowerSeries prefix(int n) const {
        assert(n >= 0);
        FormalPowerSeries result(this->begin(),
                                 this->begin() + std::min<int>(n, this->size()));
        result.resize(n);
        return result;
    }

    FormalPowerSeries pre(int n) const { return prefix(n); }

    void shrink() {
        while (!this->empty() && this->back() == Mint(0)) this->pop_back();
    }

    FormalPowerSeries& normalize() {
        shrink();
        return *this;
    }

    FormalPowerSeries reversed() const {
        FormalPowerSeries result = *this;
        std::reverse(result.begin(), result.end());
        return result;
    }

    FormalPowerSeries rev() const { return reversed(); }

    FormalPowerSeries& operator+=(Mint rhs) {
        if (this->empty()) this->resize(1);
        (*this)[0] += rhs;
        return *this;
    }

    FormalPowerSeries& operator-=(Mint rhs) {
        if (this->empty()) this->resize(1);
        (*this)[0] -= rhs;
        return *this;
    }

    FormalPowerSeries& operator+=(const FormalPowerSeries& rhs) {
        if (this->size() < rhs.size()) this->resize(rhs.size());
        for (int i = 0; i < static_cast<int>(rhs.size()); ++i) {
            (*this)[i] += rhs[i];
        }
        return *this;
    }

    FormalPowerSeries& operator-=(const FormalPowerSeries& rhs) {
        if (this->size() < rhs.size()) this->resize(rhs.size());
        for (int i = 0; i < static_cast<int>(rhs.size()); ++i) {
            (*this)[i] -= rhs[i];
        }
        return *this;
    }

    FormalPowerSeries& operator*=(const FormalPowerSeries& rhs) {
        *this = convolution_lib::convolution<Mint>(*this, rhs);
        return *this;
    }

    FormalPowerSeries& operator*=(Mint scalar) {
        for (Mint& x : *this) x *= scalar;
        return *this;
    }

    FormalPowerSeries& operator/=(Mint scalar) {
        return *this *= scalar.inv();
    }

    FormalPowerSeries operator+() const { return *this; }

    FormalPowerSeries operator-() const {
        FormalPowerSeries result = *this;
        for (Mint& x : result) x = -x;
        return result;
    }

    friend FormalPowerSeries operator+(FormalPowerSeries lhs,
                                       const FormalPowerSeries& rhs) {
        return lhs += rhs;
    }

    friend FormalPowerSeries operator+(FormalPowerSeries lhs, Mint rhs) {
        return lhs += rhs;
    }

    friend FormalPowerSeries operator+(Mint lhs, FormalPowerSeries rhs) {
        return rhs += lhs;
    }

    friend FormalPowerSeries operator-(FormalPowerSeries lhs,
                                       const FormalPowerSeries& rhs) {
        return lhs -= rhs;
    }

    friend FormalPowerSeries operator-(FormalPowerSeries lhs, Mint rhs) {
        return lhs -= rhs;
    }

    friend FormalPowerSeries operator-(Mint lhs, FormalPowerSeries rhs) {
        rhs = -rhs;
        rhs += lhs;
        return rhs;
    }

    friend FormalPowerSeries operator*(FormalPowerSeries lhs,
                                       const FormalPowerSeries& rhs) {
        return lhs *= rhs;
    }

    friend FormalPowerSeries operator*(FormalPowerSeries lhs, Mint rhs) {
        return lhs *= rhs;
    }

    friend FormalPowerSeries operator*(Mint lhs, FormalPowerSeries rhs) {
        return rhs *= lhs;
    }

    friend FormalPowerSeries operator/(FormalPowerSeries lhs, Mint rhs) {
        return lhs /= rhs;
    }

    FormalPowerSeries shifted_left(int n) const {
        assert(n >= 0);
        if (this->empty()) return {};
        FormalPowerSeries result(n, Mint(0));
        result.insert(result.end(), this->begin(), this->end());
        return result;
    }

    FormalPowerSeries shifted_right(int n) const {
        assert(n >= 0);
        if (n >= static_cast<int>(this->size())) return {};
        return FormalPowerSeries(this->begin() + n, this->end());
    }

    FormalPowerSeries operator<<(int n) const { return shifted_left(n); }

    FormalPowerSeries operator>>(int n) const { return shifted_right(n); }

    FormalPowerSeries mul_trunc(const FormalPowerSeries& rhs, int n) const {
        assert(n >= 0);
        if (n == 0 || this->empty() || rhs.empty()) {
            return FormalPowerSeries(n, Mint(0));
        }
        FormalPowerSeries a(this->begin(),
                            this->begin() + std::min<int>(n, this->size()));
        FormalPowerSeries b(rhs.begin(),
                            rhs.begin() + std::min<int>(n, rhs.size()));
        FormalPowerSeries result = convolution_lib::convolution<Mint>(a, b);
        result.resize(n);
        return result;
    }

    Mint eval(Mint x) const {
        Mint result = 0;
        for (int i = static_cast<int>(this->size()) - 1; i >= 0; --i) {
            result = result * x + (*this)[i];
        }
        return result;
    }

    FormalPowerSeries derivative() const {
        if (this->size() <= 1) return {};
        FormalPowerSeries result(this->size() - 1);
        for (int i = 1; i < static_cast<int>(this->size()); ++i) {
            result[i - 1] = (*this)[i] * Mint(i);
        }
        return result;
    }

    FormalPowerSeries diff() const { return derivative(); }

    FormalPowerSeries integral() const {
        FormalPowerSeries result(this->size() + 1);
        for (int i = 0; i < static_cast<int>(this->size()); ++i) {
            result[i + 1] = (*this)[i] / Mint(i + 1);
        }
        return result;
    }

    FormalPowerSeries inv(int n) const {
        assert(n >= 0);
        if (n == 0) return {};
        assert(!this->empty() && (*this)[0] != Mint(0));
        if (count_nonzero(n) <= sparse_threshold) return inv_sparse(n);

        FormalPowerSeries result{(*this)[0].inv()};
        for (int m = 1; m < n; m <<= 1) {
            const int size = std::min(2 * m, n);
            FormalPowerSeries correction =
                this->prefix(size).mul_trunc(result, size);
            for (Mint& x : correction) x = -x;
            correction[0] += Mint(2);
            result = result.mul_trunc(correction, size);
        }
        result.resize(n);
        return result;
    }

    FormalPowerSeries series_div(const FormalPowerSeries& denominator,
                                 int n) const {
        assert(n >= 0);
        return this->prefix(n).mul_trunc(denominator.inv(n), n);
    }

    FormalPowerSeries log(int n) const {
        assert(n >= 0);
        if (n == 0) return {};
        assert(!this->empty() && (*this)[0] == Mint(1));
        if (count_nonzero(n) <= sparse_threshold) return log_sparse(n);
        return derivative().mul_trunc(inv(n), n - 1).integral().prefix(n);
    }

    FormalPowerSeries exp(int n) const {
        assert(n >= 0);
        if (n == 0) return {};
        assert(this->empty() || (*this)[0] == Mint(0));
        if (count_nonzero(n) <= sparse_threshold) return exp_sparse(n);

        FormalPowerSeries result{Mint(1)};
        for (int m = 1; m < n; m <<= 1) {
            const int size = std::min(2 * m, n);
            FormalPowerSeries correction = this->prefix(size) - result.log(size);
            correction[0] += Mint(1);
            result = result.mul_trunc(correction, size);
        }
        result.resize(n);
        return result;
    }

    FormalPowerSeries pow(long long exponent, int n) const {
        assert(exponent >= 0);
        assert(n >= 0);
        if (n == 0) return {};
        if (exponent == 0) {
            FormalPowerSeries result(n);
            result[0] = 1;
            return result;
        }

        int first = 0;
        while (first < static_cast<int>(this->size()) &&
               (*this)[first] == Mint(0)) {
            ++first;
        }
        if (first == static_cast<int>(this->size()) ||
            first > (n - 1) / exponent) {
            return FormalPowerSeries(n, Mint(0));
        }

        const int shift = static_cast<int>(first * exponent);
        const Mint leading = (*this)[first];
        FormalPowerSeries normalized = shifted_right(first) / leading;
        FormalPowerSeries result =
            (normalized.log(n - shift) * Mint(exponent)).exp(n - shift);
        result *= leading.pow(exponent);
        result = result.shifted_left(shift);
        result.resize(n);
        return result;
    }

    std::optional<FormalPowerSeries> sqrt(int n) const {
        assert(n >= 0);
        if (n == 0) return FormalPowerSeries{};

        int first = 0;
        while (first < static_cast<int>(this->size()) &&
               (*this)[first] == Mint(0)) {
            ++first;
        }
        if (first == static_cast<int>(this->size())) {
            return FormalPowerSeries(n, Mint(0));
        }
        if (first & 1) return std::nullopt;
        if (first > 0) {
            const int shift = first / 2;
            if (shift >= n) return FormalPowerSeries(n, Mint(0));
            auto sub = shifted_right(first).sqrt(n - shift);
            if (!sub) return std::nullopt;
            FormalPowerSeries result = sub->shifted_left(shift);
            result.resize(n);
            return result;
        }

        auto root = modular_sqrt((*this)[0]);
        if (!root) return std::nullopt;
        assert(Mint::mod() != 2 && "FPS::sqrt requires an odd prime modulus");
        FormalPowerSeries result{*root};
        const Mint inv_two = Mint(2).inv();
        for (int m = 1; m < n; m <<= 1) {
            const int size = std::min(2 * m, n);
            FormalPowerSeries quotient =
                this->prefix(size).mul_trunc(result.inv(size), size);
            result = (result.prefix(size) + quotient) * inv_two;
            result.resize(size);
        }
        result.resize(n);
        return result;
    }

    std::pair<FormalPowerSeries, FormalPowerSeries>
    divmod(const FormalPowerSeries& divisor) const {
        FormalPowerSeries f = *this;
        FormalPowerSeries g = divisor;
        f.shrink();
        g.shrink();
        assert(!g.empty());
        if (f.size() < g.size()) return {{}, f};

        const int quotient_size =
            static_cast<int>(f.size() - g.size() + 1);
        FormalPowerSeries quotient =
            f.reversed()
                .prefix(quotient_size)
                .mul_trunc(g.reversed().prefix(quotient_size).inv(quotient_size),
                           quotient_size)
                .reversed();
        quotient.shrink();

        FormalPowerSeries remainder = f - quotient * g;
        remainder.resize(g.size() - 1);
        remainder.shrink();
        return {quotient, remainder};
    }

    FormalPowerSeries quotient(const FormalPowerSeries& divisor) const {
        return divmod(divisor).first;
    }

    FormalPowerSeries remainder(const FormalPowerSeries& divisor) const {
        return divmod(divisor).second;
    }

    FormalPowerSeries compose(const FormalPowerSeries& inner, int n) const {
        assert(n >= 0);
        FormalPowerSeries result(n, Mint(0));
        for (int i = static_cast<int>(this->size()) - 1; i >= 0; --i) {
            result = result.mul_trunc(inner, n);
            result[0] += (*this)[i];
        }
        return result;
    }

    FormalPowerSeries compositional_inverse(int n) const {
        assert(n >= 0);
        if (n == 0) return {};
        assert(this->size() >= 2 && (*this)[0] == Mint(0) &&
               (*this)[1] != Mint(0));

        FormalPowerSeries result(std::min(n, 2), Mint(0));
        if (n >= 2) result[1] = (*this)[1].inv();
        for (int m = 2; m < n; m <<= 1) {
            const int size = std::min(2 * m, n);
            FormalPowerSeries error = compose(result, size);
            if (size >= 2) error[1] -= Mint(1);
            FormalPowerSeries jacobian = derivative().compose(result, size);
            result =
                (result.prefix(size) - error.series_div(jacobian, size)).prefix(size);
        }
        result.resize(n);
        return result;
    }

    FormalPowerSeries taylor_shift(Mint c) const {
        const int n = static_cast<int>(this->size());
        if (n == 0) return {};

        std::vector<Mint> fact(n, Mint(1));
        std::vector<Mint> inv_fact(n, Mint(1));
        for (int i = 1; i < n; ++i) fact[i] = fact[i - 1] * Mint(i);
        inv_fact[n - 1] = fact[n - 1].inv();
        for (int i = n - 1; i >= 1; --i) {
            inv_fact[i - 1] = inv_fact[i] * Mint(i);
        }

        FormalPowerSeries a(n), b(n);
        for (int i = 0; i < n; ++i) a[n - 1 - i] = (*this)[i] * fact[i];
        Mint power = 1;
        for (int i = 0; i < n; ++i) {
            b[i] = power * inv_fact[i];
            power *= c;
        }

        FormalPowerSeries product = convolution_lib::convolution<Mint>(a, b);
        FormalPowerSeries result(n);
        for (int i = 0; i < n; ++i) {
            result[i] = product[n - 1 - i] * inv_fact[i];
        }
        return result;
    }

    std::vector<Mint> multipoint_evaluate(const std::vector<Mint>& points) const {
        const int count = static_cast<int>(points.size());
        if (count == 0) return {};

        int size = 1;
        while (size < count) size <<= 1;
        std::vector<FormalPowerSeries> tree(2 * size, FormalPowerSeries{Mint(1)});
        for (int i = 0; i < count; ++i) {
            tree[size + i] = FormalPowerSeries{-points[i], Mint(1)};
        }
        for (int i = size - 1; i >= 1; --i) {
            tree[i] = tree[2 * i] * tree[2 * i + 1];
        }

        std::vector<FormalPowerSeries> rem(2 * size);
        rem[1] = remainder(tree[1]);
        for (int i = 1; i < size; ++i) {
            rem[2 * i] = rem[i].remainder(tree[2 * i]);
            rem[2 * i + 1] = rem[i].remainder(tree[2 * i + 1]);
        }

        std::vector<Mint> result(count);
        for (int i = 0; i < count; ++i) {
            result[i] = rem[size + i].empty() ? Mint(0) : rem[size + i][0];
        }
        return result;
    }

  private:
    static constexpr int sparse_threshold = 60;

    FormalPowerSeries inv_sparse(int n) const {
        std::vector<std::pair<int, Mint>> terms;
        for (int i = 1; i < std::min<int>(n, this->size()); ++i) {
            if ((*this)[i] != Mint(0)) terms.emplace_back(i, (*this)[i]);
        }

        FormalPowerSeries result(n);
        result[0] = (*this)[0].inv();
        for (int i = 1; i < n; ++i) {
            Mint value = 0;
            for (const auto& [degree, coefficient] : terms) {
                if (degree > i) break;
                value -= coefficient * result[i - degree];
            }
            result[i] = value * result[0];
        }
        return result;
    }

    FormalPowerSeries log_sparse(int n) const {
        std::vector<std::pair<int, Mint>> terms;
        for (int i = 1; i < std::min<int>(n, this->size()); ++i) {
            if ((*this)[i] != Mint(0)) terms.emplace_back(i, (*this)[i]);
        }

        FormalPowerSeries result(n);
        FormalPowerSeries quotient(std::max(0, n - 1));
        for (int i = 0; i + 1 < n; ++i) {
            Mint value =
                i + 1 < static_cast<int>(this->size())
                    ? (*this)[i + 1] * Mint(i + 1)
                    : Mint(0);
            for (const auto& [degree, coefficient] : terms) {
                if (degree > i) break;
                value -= coefficient * quotient[i - degree];
            }
            quotient[i] = value;
            result[i + 1] = value / Mint(i + 1);
        }
        return result;
    }

    FormalPowerSeries exp_sparse(int n) const {
        std::vector<std::pair<int, Mint>> derivative_terms;
        for (int i = 1; i < std::min<int>(n, this->size()); ++i) {
            if ((*this)[i] != Mint(0)) {
                derivative_terms.emplace_back(i, (*this)[i] * Mint(i));
            }
        }

        FormalPowerSeries result(n);
        result[0] = 1;
        for (int i = 1; i < n; ++i) {
            Mint value = 0;
            for (const auto& [degree, coefficient] : derivative_terms) {
                if (degree > i) break;
                value += coefficient * result[i - degree];
            }
            result[i] = value / Mint(i);
        }
        return result;
    }

    static std::optional<Mint> modular_sqrt(Mint value) {
        const int mod = Mint::mod();
        assert(modint_lib::internal::is_prime_constexpr(mod));
        if (value == Mint(0)) return Mint(0);
        if (mod == 2) return value;
        if (value.pow((mod - 1) / 2) != Mint(1)) return std::nullopt;
        if (mod % 4 == 3) return value.pow((mod + 1) / 4);

        long long q = mod - 1;
        int s = 0;
        while ((q & 1) == 0) {
            q >>= 1;
            ++s;
        }

        Mint z = 2;
        while (z.pow((mod - 1) / 2) == Mint(1)) z += Mint(1);
        Mint c = z.pow(q);
        Mint x = value.pow((q + 1) / 2);
        Mint t = value.pow(q);
        int m = s;
        while (t != Mint(1)) {
            int i = 1;
            Mint squared = t * t;
            while (i < m && squared != Mint(1)) {
                squared *= squared;
                ++i;
            }
            assert(i < m);
            Mint b = c.pow(1LL << (m - i - 1));
            x *= b;
            c = b * b;
            t *= c;
            m = i;
        }
        return x;
    }
};

template <class Mint>
using FPS = FormalPowerSeries<Mint>;

template <class Mint>
FormalPowerSeries<Mint> interpolate(const std::vector<Mint>& points,
                                    const std::vector<Mint>& values) {
    assert(points.size() == values.size());
    const int count = static_cast<int>(points.size());
    if (count == 0) return {};

    int size = 1;
    while (size < count) size <<= 1;
    std::vector<FormalPowerSeries<Mint>> tree(
        2 * size, FormalPowerSeries<Mint>{Mint(1)});
    for (int i = 0; i < count; ++i) {
        tree[size + i] = FormalPowerSeries<Mint>{-points[i], Mint(1)};
    }
    for (int i = size - 1; i >= 1; --i) {
        tree[i] = tree[2 * i] * tree[2 * i + 1];
    }

    std::vector<Mint> denominators =
        tree[1].derivative().multipoint_evaluate(points);
    std::vector<FormalPowerSeries<Mint>> result(2 * size);
    for (int i = 0; i < count; ++i) {
        assert(denominators[i] != Mint(0));
        result[size + i] =
            FormalPowerSeries<Mint>{values[i] / denominators[i]};
    }
    for (int i = size - 1; i >= 1; --i) {
        result[i] = result[2 * i] * tree[2 * i + 1] +
                    result[2 * i + 1] * tree[2 * i];
    }
    result[1].shrink();
    return result[1];
}

template <class Mint>
Mint bostan_mori(FormalPowerSeries<Mint> numerator,
                 FormalPowerSeries<Mint> denominator, long long n) {
    assert(n >= 0);
    assert(!denominator.empty() && denominator[0] != Mint(0));
    if (numerator.empty()) return Mint(0);

    while (n > 0) {
        FormalPowerSeries<Mint> denominator_neg = denominator;
        for (int i = 1; i < static_cast<int>(denominator_neg.size()); i += 2) {
            denominator_neg[i] = -denominator_neg[i];
        }
        FormalPowerSeries<Mint> product_p = numerator * denominator_neg;
        FormalPowerSeries<Mint> product_q = denominator * denominator_neg;

        FormalPowerSeries<Mint> next_p;
        for (int i = static_cast<int>(n & 1); i < static_cast<int>(product_p.size());
             i += 2) {
            next_p.push_back(product_p[i]);
        }
        FormalPowerSeries<Mint> next_q;
        for (int i = 0; i < static_cast<int>(product_q.size()); i += 2) {
            next_q.push_back(product_q[i]);
        }
        numerator = std::move(next_p);
        denominator = std::move(next_q);
        n >>= 1;
    }
    return numerator.empty() ? Mint(0) : numerator[0] / denominator[0];
}

template <class Mint>
Mint linear_recurrence_nth(const std::vector<Mint>& initial,
                           const std::vector<Mint>& recurrence, long long n) {
    assert(n >= 0);
    assert(initial.size() == recurrence.size());
    const int order = static_cast<int>(recurrence.size());
    assert(order > 0);
    if (n < order) return initial[n];

    FormalPowerSeries<Mint> denominator(order + 1);
    denominator[0] = 1;
    for (int i = 0; i < order; ++i) denominator[i + 1] = -recurrence[i];
    FormalPowerSeries<Mint> numerator =
        FormalPowerSeries<Mint>(initial).mul_trunc(denominator, order);
    return bostan_mori(numerator, denominator, n);
}

template <class Mint>
std::vector<Mint> berlekamp_massey(const std::vector<Mint>& sequence) {
    std::vector<Mint> current{Mint(1)};
    std::vector<Mint> previous{Mint(1)};
    int length = 0;
    int shift = 1;
    Mint last_discrepancy = 1;

    for (int n = 0; n < static_cast<int>(sequence.size()); ++n) {
        Mint discrepancy = sequence[n];
        for (int i = 1; i <= length; ++i) {
            discrepancy += current[i] * sequence[n - i];
        }
        if (discrepancy == Mint(0)) {
            ++shift;
            continue;
        }

        std::vector<Mint> saved = current;
        Mint coefficient = discrepancy / last_discrepancy;
        if (current.size() < previous.size() + shift) {
            current.resize(previous.size() + shift);
        }
        for (int i = 0; i < static_cast<int>(previous.size()); ++i) {
            current[i + shift] -= coefficient * previous[i];
        }

        if (2 * length <= n) {
            length = n + 1 - length;
            previous = std::move(saved);
            last_discrepancy = discrepancy;
            shift = 1;
        } else {
            ++shift;
        }
    }

    std::vector<Mint> recurrence(length);
    for (int i = 0; i < length; ++i) recurrence[i] = -current[i + 1];
    return recurrence;
}

} // namespace formal_power_series_lib

#endif // FORMAL_POWER_SERIES_LIBRARY_HPP
// clang-format on
// ===== END FORMAL POWER SERIES COPY BLOCK =====
