#pragma once

#include <set>

namespace set_operations_lib
{
    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> operator|(const std::set<T, CompareA, AllocA> &a,
                                            const std::set<T, CompareB, AllocB> &b)
    {
        std::set<T, CompareA, AllocA> res(a.begin(), a.end(), a.key_comp(), a.get_allocator());
        res.insert(b.begin(), b.end());
        return res;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> operator&(const std::set<T, CompareA, AllocA> &a,
                                            const std::set<T, CompareB, AllocB> &b)
    {
        std::set<T, CompareA, AllocA> res(a.key_comp(), a.get_allocator());
        for (const auto &x : a)
        {
            if (b.find(x) != b.end())
                res.insert(x);
        }
        return res;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> operator-(const std::set<T, CompareA, AllocA> &a,
                                            const std::set<T, CompareB, AllocB> &b)
    {
        std::set<T, CompareA, AllocA> res(a.key_comp(), a.get_allocator());
        for (const auto &x : a)
        {
            if (b.find(x) == b.end())
                res.insert(x);
        }
        return res;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> operator^(const std::set<T, CompareA, AllocA> &a,
                                            const std::set<T, CompareB, AllocB> &b)
    {
        std::set<T, CompareA, AllocA> res = a - b;
        for (const auto &x : b)
        {
            if (a.find(x) == a.end())
                res.insert(x);
        }
        return res;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> &operator|=(std::set<T, CompareA, AllocA> &a,
                                             const std::set<T, CompareB, AllocB> &b)
    {
        a = a | b;
        return a;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> &operator&=(std::set<T, CompareA, AllocA> &a,
                                             const std::set<T, CompareB, AllocB> &b)
    {
        a = a & b;
        return a;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> &operator-=(std::set<T, CompareA, AllocA> &a,
                                             const std::set<T, CompareB, AllocB> &b)
    {
        a = a - b;
        return a;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    std::set<T, CompareA, AllocA> &operator^=(std::set<T, CompareA, AllocA> &a,
                                             const std::set<T, CompareB, AllocB> &b)
    {
        a = a ^ b;
        return a;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    bool is_subset(const std::set<T, CompareA, AllocA> &a,
                   const std::set<T, CompareB, AllocB> &b)
    {
        for (const auto &x : a)
        {
            if (b.find(x) == b.end())
                return false;
        }
        return true;
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    bool is_superset(const std::set<T, CompareA, AllocA> &a,
                     const std::set<T, CompareB, AllocB> &b)
    {
        return is_subset(b, a);
    }

    template <class T, class CompareA, class AllocA, class CompareB, class AllocB>
    bool is_disjoint(const std::set<T, CompareA, AllocA> &a,
                     const std::set<T, CompareB, AllocB> &b)
    {
        for (const auto &x : a)
        {
            if (b.find(x) != b.end())
                return false;
        }
        return true;
    }
} // namespace set_operations_lib
