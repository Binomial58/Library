#pragma once

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

    private:
        ll max_n_ = -1;
        std::vector<char> is_prime_;
        std::vector<ll> primes_;
    };
} // namespace sieve_lib
