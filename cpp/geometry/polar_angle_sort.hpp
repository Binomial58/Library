#pragma once

#include <utility>

namespace polar_angle_lib
{
    using ll = long long;
    using Point = std::pair<ll, ll>;

    // 外積 a.x * b.y - a.y * b.x
    // > 0: aからbへ反時計回りに角度差(0°, 180°)の範囲
    // < 0: aからbへ時計回りに角度差(0°, 180°)の範囲（= bからaが反時計回り）
    // == 0: a, bは同一直線上（同じ方向 or 正反対の方向。詳細はsameAngle参照）
    inline ll cross(const Point &a, const Point &b)
    {
        return a.first * b.second - a.second * b.first;
    }

    // 半平面判定（内部で使用）。[0°, 180°) を 0、[180°, 360°) を 1 とする。
    // 境界は 0° が 0 側、180° が 1 側に入る（半開区間）。
    inline bool ah(const Point &p)
    {
        return (p.second < 0) || (p.second == 0 && p.first < 0);
    }

    // 偏角[0°, 360°)の反時計回り順に並べるための比較関数。
    // std::sort(points.begin(), points.end(), cmp) のように使う。
    // strict weak ordering になる。
    inline bool cmp(const Point &a, const Point &b)
    {
        const bool ah_a = ah(a);
        const bool ah_b = ah(b);
        if (ah_a != ah_b)
            return ah_a < ah_b;
        return cross(a, b) > 0;
    }

    // aとbが厳密に同じ方向（偏角が完全一致）かどうかを判定する。
    // 注意: cross(a, b) == 0 だけでは同一直線上（同じ方向 or 正反対）としか分からない。
    // 「同じ方向かどうか」を判定したいときは必ずこちらを使うこと。
    inline bool sameAngle(const Point &a, const Point &b)
    {
        return ah(a) == ah(b) && cross(a, b) == 0;
    }
} // namespace polar_angle_lib
