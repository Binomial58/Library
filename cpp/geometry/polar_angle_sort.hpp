#pragma once

#include <utility>

namespace polar_angle_lib
{
    using ll = long long;
    using Point = std::pair<ll, ll>;

    // 外積 a.x * b.y - a.y * b.x（符号の意味はpolar_angle_sort.md参照）
    inline ll cross(const Point &a, const Point &b)
    {
        return a.first * b.second - a.second * b.first;
    }

    // 半平面判定（内部で使用）。[0°, 180°) を 0、[180°, 360°) を 1 とする
    inline bool ah(const Point &p)
    {
        return (p.second < 0) || (p.second == 0 && p.first < 0);
    }

    // 偏角[0°, 360°)の反時計回り順の比較関数。std::sortに渡す
    inline bool cmp(const Point &a, const Point &b)
    {
        const bool ah_a = ah(a);
        const bool ah_b = ah(b);
        if (ah_a != ah_b)
            return ah_a < ah_b;
        return cross(a, b) > 0;
    }

    // aとbが厳密に同じ方向か（cross==0だけでは正反対も含むので注意）
    inline bool sameAngle(const Point &a, const Point &b)
    {
        return ah(a) == ah(b) && cross(a, b) == 0;
    }
} // namespace polar_angle_lib
