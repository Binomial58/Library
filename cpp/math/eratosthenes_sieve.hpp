#pragma once

#include <algorithm>
#include <utility>
#include <vector>
using ll = long long;

namespace sieve_lib
{
    class EratosthenesSieve
    {
    public:
        EratosthenesSieve() = default;
        explicit EratosthenesSieve(ll max_n)
        {
            build(max_n);
        }

        // 0..max_n の範囲で篩を構築する
        void build(ll max_n)
        {
            if (max_n < 0)
                max_n = 0;
            max_n_ = max_n;
            is_prime_.assign(max_n_ + 1, true);
            primes_.clear();

            is_prime_[0] = false;
            if (max_n_ >= 1)
                is_prime_[1] = false;

            for (int i = 2; 1LL * i * i <= max_n_; ++i)
            {
                if (!is_prime_[i])
                    continue;
                for (long long j = 1LL * i * i; j <= max_n_; j += i)
                {
                    is_prime_[static_cast<ll>(j)] = false;
                }
            }

            for (int x = 2; x <= max_n_; ++x)
            {
                if (is_prime_[x])
                    primes_.push_back(x);
            }

            // 最小素因数（SPF）: is_prime_ / primes_ とは独立に計算する
            spf_.assign(max_n_ + 1, 0);
            for (ll p : primes_)
            {
                for (ll j = p; j <= max_n_; j += p)
                {
                    if (spf_[static_cast<std::size_t>(j)] == 0)
                        spf_[static_cast<std::size_t>(j)] = static_cast<int>(p);
                }
            }
        }

        // x が素数かどうか（範囲外は false）
        bool is_prime(ll x) const
        {
            if (x < 0 || x > max_n_)
                return false;
            return is_prime_[x];
        }

        // 構築済み範囲の素数一覧
        const std::vector<ll> &primes() const
        {
            return primes_;
        }

        // 篩の上限
        ll max_n() const
        {
            return max_n_;
        }

        // x（2 <= x <= max_n）の最小の素因数を返す。O(1)。
        // 範囲外・x < 2 のときは 0 を返す。
        int smallest_prime_factor(ll x) const
        {
            if (x < 2 || x > max_n_)
                return 0;
            return spf_[static_cast<std::size_t>(x)];
        }

        // x（1 <= x <= max_n）を素因数分解し、(素数, 指数) のペアを素数の昇順で返す。
        // x == 1 のときは空の配列。範囲外（x < 1 または x > max_n）も空の配列を返す。
        // O(log x)。
        std::vector<std::pair<ll, int>> factorize(ll x) const
        {
            std::vector<std::pair<ll, int>> res;
            if (x < 1 || x > max_n_)
                return res;

            while (x > 1)
            {
                const int p = spf_[static_cast<std::size_t>(x)];
                int cnt = 0;
                while (spf_[static_cast<std::size_t>(x)] == p)
                {
                    x /= p;
                    ++cnt;
                }
                res.emplace_back(p, cnt);
            }
            return res;
        }

        // x（1 <= x <= max_n）の約数を昇順で列挙する。factorize を利用する。
        std::vector<ll> divisors(ll x) const
        {
            std::vector<ll> res;
            if (x < 1 || x > max_n_)
                return res;

            res.push_back(1);
            for (auto [p, e] : factorize(x))
            {
                std::vector<ll> cur = res;
                ll mul = 1;
                for (int i = 1; i <= e; ++i)
                {
                    mul *= p;
                    for (ll v : res)
                        cur.push_back(v * mul);
                }
                res.swap(cur);
            }
            std::sort(res.begin(), res.end());
            return res;
        }

    private:
        ll max_n_ = -1;
        std::vector<char> is_prime_;
        std::vector<ll> primes_;
        std::vector<int> spf_;
    };
} // namespace sieve_lib
