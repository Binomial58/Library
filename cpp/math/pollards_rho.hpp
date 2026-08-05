#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <map>
#include <random>
#include <utility>
#include <vector>

namespace pollards_rho
{
    using ull = unsigned long long;
    using u128 = __uint128_t;

    // 最大公約数（ユークリッド）
    inline ull gcd(ull a, ull b)
    {
        while (a)
        {
            b %= a;
            std::swap(a, b);
        }
        return b;
    }

    // a*b mod mod（オーバーフロー回避）
    inline ull modmul(ull a, ull b, ull mod)
    {
        return (u128)a * b % mod;
    }

    // a^e mod mod（二分累乗）
    inline ull modpow(ull a, ull e, ull mod)
    {
        ull r = 1;
        while (e)
        {
            if (e & 1)
                r = modmul(r, a, mod);
            a = modmul(a, a, mod);
            e >>= 1;
        }
        return r;
    }

    // Miller-Rabin（64bit決定版）
    inline bool is_prime(ull n)
    {
        if (n < 2)
            return false;
        if (n == 2)
            return true;
        if ((n & 1) == 0)
            return false;

        ull d = n - 1;
        int s = 0;
        while ((d & 1) == 0)
        {
            d >>= 1;
            ++s;
        }

        static const ull test_numbers[] = {
            2, 3, 5, 7, 11, 13, 17, 19,
            23, 29, 31, 37};

        for (ull a : test_numbers)
        {
            if (a >= n)
                continue;
            ull x = modpow(a, d, n);
            if (x == 1 || x == n - 1)
                continue;
            bool comp = true;
            for (int r = 1; r < s; ++r)
            {
                x = modmul(x, x, n);
                if (x == n - 1)
                {
                    comp = false;
                    break;
                }
            }
            if (comp)
                return false;
        }
        return true;
    }

    // Pollard's Rho で n の非自明因子を1つ返す
    inline ull find_prime_factor(ull n)
    {
        if ((n & 1) == 0)
            return 2;
        if (is_prime(n))
            return n;

        static std::mt19937_64 rng(
            std::chrono::steady_clock::now().time_since_epoch().count());

        while (true)
        {
            std::uniform_int_distribution<ull> dist_c(1, n - 1);
            std::uniform_int_distribution<ull> dist_x(0, n - 1);
            ull c = dist_c(rng);
            ull x = dist_x(rng);
            ull y = x;
            ull g = 1;

            auto f = [&](ull v)
            {
                return (modmul(v, v, n) + c) % n;
            };

            while (g == 1)
            {
                x = f(x);
                y = f(f(y));
                ull diff = (x > y) ? (x - y) : (y - x);
                g = gcd(diff, n);
            }
            if (g != n)
                return g;
        }
    }

    // 素因数分解（n = 1 のときは空）
    inline std::map<ull, int> factorize(ull n)
    {
        std::map<ull, int> res;
        if (n <= 1)
            return res;
        if (is_prime(n))
        {
            res[n]++;
            return res;
        }
        ull p = find_prime_factor(n);
        auto l = factorize(p);
        auto r = factorize(n / p);
        for (auto &e : l)
            res[e.first] += e.second;
        for (auto &e : r)
            res[e.first] += e.second;
        return res;
    }

    // 約数列挙（昇順）
    inline std::vector<ull> divisors(ull n)
    {
        std::vector<ull> res;
        if (n == 0)
            return res;
        auto mp = factorize(n);
        res.push_back(1);
        for (auto &kv : mp)
        {
            ull p = kv.first;
            int e = kv.second;
            std::vector<ull> cur = res;
            ull mul = 1;
            for (int i = 1; i <= e; ++i)
            {
                mul *= p;
                for (ull v : res)
                    cur.push_back(v * mul);
            }
            res.swap(cur);
        }
        std::sort(res.begin(), res.end());
        return res;
    }

} // namespace pollards_rho
