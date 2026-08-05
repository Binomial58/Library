#pragma once

#include <algorithm>
#include <limits>
#include <string>
#include <string_view>

namespace base_conversion_lib
{
    enum class ParseStatus
    {
        ok,
        invalid_base,
        empty_input,
        invalid_digit,
        overflow,
    };

    struct ParseU64Result
    {
        ParseStatus status = ParseStatus::ok;
        unsigned long long value = 0;
        std::size_t error_pos = std::string_view::npos;

        explicit operator bool() const
        {
            return status == ParseStatus::ok;
        }
    };

    inline bool is_valid_base(int base)
    {
        return 2 <= base && base <= 36;
    }

    inline int digit_to_value(char c)
    {
        if ('0' <= c && c <= '9')
            return c - '0';
        if ('A' <= c && c <= 'Z')
            return c - 'A' + 10;
        if ('a' <= c && c <= 'z')
            return c - 'a' + 10;
        return -1;
    }

    inline char value_to_digit(int value)
    {
        if (0 <= value && value <= 9)
            return static_cast<char>('0' + value);
        return static_cast<char>('A' + (value - 10));
    }

    inline const char *parse_status_message(ParseStatus status)
    {
        if (status == ParseStatus::ok)
            return "ok";
        if (status == ParseStatus::invalid_base)
            return "invalid_base";
        if (status == ParseStatus::empty_input)
            return "empty_input";
        if (status == ParseStatus::invalid_digit)
            return "invalid_digit";
        if (status == ParseStatus::overflow)
            return "overflow";
        return "unknown";
    }

    // base-k 文字列 -> 10進(unsigned long long)
    // - base は [2, 36]
    // - 許可文字は 0-9, A-Z, a-z
    // - 先頭の '+' は許可（'-' は不許可）
    // - オーバーフロー時は ParseStatus::overflow
    inline ParseU64Result k_to_10(std::string_view s, int base)
    {
        ParseU64Result result;
        if (!is_valid_base(base))
        {
            result.status = ParseStatus::invalid_base;
            return result;
        }
        if (s.empty())
        {
            result.status = ParseStatus::empty_input;
            return result;
        }

        std::size_t i = 0;
        if (s[0] == '+')
        {
            i = 1;
            if (i == s.size())
            {
                result.status = ParseStatus::empty_input;
                result.error_pos = 0;
                return result;
            }
        }

        unsigned long long value = 0;
        const unsigned long long max_u64 = std::numeric_limits<unsigned long long>::max();

        for (; i < s.size(); ++i)
        {
            const int d = digit_to_value(s[i]);
            if (d < 0 || d >= base)
            {
                result.status = ParseStatus::invalid_digit;
                result.error_pos = i;
                return result;
            }

            if (value > (max_u64 - static_cast<unsigned long long>(d)) / static_cast<unsigned long long>(base))
            {
                result.status = ParseStatus::overflow;
                result.error_pos = i;
                return result;
            }

            value = value * static_cast<unsigned long long>(base) + static_cast<unsigned long long>(d);
        }

        result.value = value;
        return result;
    }

    inline bool k_to_10(std::string_view s, int base, unsigned long long &out)
    {
        const ParseU64Result result = k_to_10(s, base);
        if (!result)
            return false;
        out = result.value;
        return true;
    }

    // 10進(unsigned long long) -> base-n 文字列
    // - base は [2, 36]
    // - 不正な base のときは空文字列を返す
    inline std::string ten_to_n(unsigned long long value, int base)
    {
        if (!is_valid_base(base))
            return "";

        if (value == 0)
            return "0";

        std::string s;
        while (value > 0)
        {
            const int d = static_cast<int>(value % static_cast<unsigned long long>(base));
            s.push_back(value_to_digit(d));
            value /= static_cast<unsigned long long>(base);
        }
        std::reverse(s.begin(), s.end());
        return s;
    }

    inline bool ten_to_n(unsigned long long value, int base, std::string &out)
    {
        if (!is_valid_base(base))
        {
            out.clear();
            return false;
        }
        out = ten_to_n(value, base);
        return true;
    }
} // namespace base_conversion_lib
