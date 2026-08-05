#pragma once

#include <string>
#include <vector>

namespace next_occurrence_lib
{
    // nex[i][c] := the first index >= i where char('a' + c) appears.
    // If it does not appear, nex[i][c] == s.size().
    inline std::vector<std::vector<int>> calc_next_occurrence(const std::string &s)
    {
        const int n = static_cast<int>(s.size());
        std::vector<std::vector<int>> nex(n + 1, std::vector<int>(26, n));

        for (int i = n - 1; i >= 0; --i)
        {
            nex[i] = nex[i + 1];
            nex[i][s[i] - 'a'] = i;
        }

        return nex;
    }

    inline std::vector<std::vector<int>> calc_nex(const std::string &s)
    {
        return calc_next_occurrence(s);
    }
} // namespace next_occurrence_lib
