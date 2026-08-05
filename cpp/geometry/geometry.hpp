#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <stdexcept>
#include <utility>
#include <vector>

namespace geometry
{

using Real = double;
using Point = std::complex<Real>;
using Line = std::pair<Point, Point>;
using Segment = std::pair<Point, Point>;
using Polygon = std::vector<Point>;

struct Circle
{
    Point center;
    Real radius;
};

struct Edge
{
    int from;
    int to;
    Real cost;
};

struct Graph
{
    int n;
    std::vector<std::vector<Edge>> edges;

    Graph() = default;
    explicit Graph(int n) : n(n), edges(n) {}

    void add_edge(int from, int to, Real cost)
    {
        edges[from].push_back({from, to, cost});
        edges[to].push_back({to, from, cost});
    }
};

enum Containment
{
    OUT = 0,
    IN = 1,
    ON = 2,
};

inline constexpr Real EPS = 1e-9;
inline constexpr Real PI = 3.141592653589793238462643383279502884L;

inline int sgn(Real x)
{
    if (x > EPS)
        return 1;
    if (x < -EPS)
        return -1;
    return 0;
}

inline bool eq(Real a, Real b)
{
    return std::abs(a - b) < EPS;
}

inline bool eq(Point a, Point b)
{
    return std::abs(a - b) < EPS;
}

inline bool point_less(Point a, Point b)
{
    if (!eq(a.real(), b.real()))
        return a.real() < b.real();
    return a.imag() < b.imag() - EPS;
}

inline Real dot(Point a, Point b)
{
    return std::real(std::conj(a) * b);
}

inline Real cross(Point a, Point b)
{
    return std::imag(std::conj(a) * b);
}

inline Real norm2(Point a)
{
    return dot(a, a);
}

inline Point rotate90(Point a)
{
    return a * Point(0.0, 1.0);
}

inline Point rotate(Point a, Real rad)
{
    return a * std::polar<Real>(1.0, rad);
}

inline int ccw(Point a, Point b, Point c)
{
    b -= a;
    c -= a;
    const Real cr = cross(b, c);
    if (cr > EPS)
        return +1;
    if (cr < -EPS)
        return -1;
    if (dot(b, c) < -EPS)
        return +2;
    if (norm2(b) + EPS < norm2(c))
        return -2;
    return 0;
}

inline bool is_intersect_lp(Point a1, Point a2, Point p)
{
    return sgn(cross(a2 - a1, p - a1)) == 0;
}

inline bool is_intersect_ll(Point a1, Point a2, Point b1, Point b2)
{
    const Real det = cross(a2 - a1, b2 - b1);
    if (sgn(det) != 0)
        return true;
    return sgn(cross(a2 - a1, b1 - a1)) == 0;
}

inline bool is_intersect_ls(Point a1, Point a2, Point b1, Point b2)
{
    return sgn(cross(a2 - a1, b1 - a1)) * sgn(cross(a2 - a1, b2 - a1)) <= 0;
}

inline bool is_intersect_ss(Point a1, Point a2, Point b1, Point b2)
{
    return ccw(a1, a2, b1) * ccw(a1, a2, b2) <= 0 &&
           ccw(b1, b2, a1) * ccw(b1, b2, a2) <= 0;
}

inline bool is_intersect_sp(Point a1, Point a2, Point p)
{
    return ccw(a1, a2, p) == 0;
}

inline Point projection(Point a1, Point a2, Point p)
{
    return a1 + dot(a2 - a1, p - a1) / norm2(a2 - a1) * (a2 - a1);
}

inline Point reflection(Point a1, Point a2, Point p)
{
    return projection(a1, a2, p) * 2.0 - p;
}

inline Real dist_lp(Point a1, Point a2, Point p)
{
    return std::abs(projection(a1, a2, p) - p);
}

inline Real dist_ll(Point a1, Point a2, Point b1, Point b2)
{
    return is_intersect_ll(a1, a2, b1, b2) ? 0.0 : dist_lp(a1, a2, b1);
}

inline Real dist_ls(Point a1, Point a2, Point b1, Point b2)
{
    return is_intersect_ls(a1, a2, b1, b2) ? 0.0 : std::min(dist_lp(a1, a2, b1), dist_lp(a1, a2, b2));
}

inline Real dist_sp(Point a1, Point a2, Point p)
{
    const Point r = projection(a1, a2, p);
    if (is_intersect_sp(a1, a2, r))
        return std::abs(r - p);
    return std::min(std::abs(a1 - p), std::abs(a2 - p));
}

inline Real dist_ss(Point a1, Point a2, Point b1, Point b2)
{
    if (is_intersect_ss(a1, a2, b1, b2))
        return 0.0;
    return std::min(
        std::min(dist_sp(a1, a2, b1), dist_sp(a1, a2, b2)),
        std::min(dist_sp(b1, b2, a1), dist_sp(b1, b2, a2)));
}

inline Point crosspoint_ll(Point a1, Point a2, Point b1, Point b2)
{
    const Real d1 = cross(b2 - b1, b1 - a1);
    const Real d2 = cross(b2 - b1, a2 - a1);
    if (eq(d1, 0.0) && eq(d2, 0.0))
        return a1;
    if (eq(d2, 0.0))
        throw std::runtime_error("geometry::crosspoint_ll: parallel lines");
    return a1 + (a2 - a1) * (d1 / d2);
}

inline Real dist_lc(Point a1, Point a2, Point c, Real r)
{
    return std::max(dist_lp(a1, a2, c) - r, 0.0);
}

inline Real dist_sc(Point a1, Point a2, Point c, Real r)
{
    const bool in1 = norm2(c - a1) < r * r - EPS;
    const bool in2 = norm2(c - a2) < r * r - EPS;
    if (in1 != in2)
        return 0.0;
    if (in1 && in2)
        return r - std::sqrt(std::max(norm2(c - a1), norm2(c - a2)));
    return std::max(dist_sp(a1, a2, c) - r, 0.0);
}

inline std::vector<Point> crosspoint_lc(Point a1, Point a2, Point c, Real r)
{
    std::vector<Point> res;
    const Point foot = projection(a1, a2, c);
    const Real d2 = norm2(foot - c);
    if (d2 > r * r + EPS)
        return res;

    const Point dir = (a2 - a1) / std::abs(a2 - a1);
    const Real len = std::sqrt(std::max(r * r - d2, 0.0));
    res.push_back(foot + dir * len);
    if (len > EPS)
        res.push_back(foot - dir * len);
    return res;
}

inline Real dist_cc(Point a, Real ar, Point b, Real br)
{
    const Real d = std::abs(a - b);
    if (d > ar + br)
        return d - ar - br;
    if (d < std::abs(ar - br))
        return std::abs(ar - br) - d;
    return 0.0;
}

inline std::vector<Point> crosspoint_cc(Point a, Real ar, Point b, Real br)
{
    std::vector<Point> res;
    const Point ab = b - a;
    const Real d = std::abs(ab);
    if (d < EPS)
        return res;

    const Real x = (d * d + ar * ar - br * br) / (2.0 * d);
    const Real y2 = ar * ar - x * x;
    if (y2 < -EPS)
        return res;

    const Point base = a + ab * (x / d);
    const Point unit = rotate90(ab / d);
    const Real y = std::sqrt(std::max(y2, 0.0));
    res.push_back(base + unit * y);
    if (y > EPS)
        res.push_back(base - unit * y);
    return res;
}

inline std::vector<Point> tangent_points(Point c, Real r, Point p)
{
    const Real d2 = norm2(p - c);
    if (d2 < r * r - EPS)
        return {};
    return crosspoint_cc(c, r, p, std::sqrt(std::max(d2 - r * r, 0.0)));
}

inline std::vector<Line> tangent_lines(Circle a, Circle b)
{
    std::vector<Line> res;
    const Point diff = b.center - a.center;
    const Real z = norm2(diff);
    if (z < EPS)
        return res;

    for (int s : {-1, 1})
    {
        const Real r = a.radius - s * b.radius;
        const Real d = z - r * r;
        if (d < -EPS)
            continue;
        const Point v = diff * (r / z);
        const Point h = rotate90(diff) * (std::sqrt(std::max(d, 0.0)) / z);
        for (int t : {-1, 1})
        {
            const Point n = v + h * static_cast<Real>(t);
            res.push_back({a.center + n * a.radius, b.center + n * (b.radius * s)});
            if (d <= EPS)
                break;
        }
    }
    return res;
}

inline Point circumcenter(Point a, Point b, Point c)
{
    const Point ab_mid = (a + b) * 0.5;
    const Point ac_mid = (a + c) * 0.5;
    return crosspoint_ll(ab_mid, ab_mid + rotate90(b - a), ac_mid, ac_mid + rotate90(c - a));
}

inline std::vector<Point> circles_points_radius(Point a, Point b, Real r)
{
    std::vector<Point> res;
    const Point mid = (a + b) * 0.5;
    const Real half = std::abs(a - b) * 0.5;
    if (half < EPS || half > r + EPS)
        return res;

    const Real h = std::sqrt(std::max(r * r - half * half, 0.0));
    const Point dir = rotate90((b - a) / std::abs(b - a));
    res.push_back(mid + dir * h);
    if (h > EPS)
        res.push_back(mid - dir * h);
    return res;
}

inline Point minimum_enclosing_circle(const Polygon &ps)
{
    Point c(0.0, 0.0);
    Real move = 0.5;
    for (int i = 0; i < 39; ++i)
    {
        for (int t = 0; t < 50; ++t)
        {
            Real best = -1.0;
            int idx = 0;
            for (int j = 0; j < static_cast<int>(ps.size()); ++j)
            {
                const Real cur = norm2(ps[j] - c);
                if (cur > best)
                {
                    best = cur;
                    idx = j;
                }
            }
            c += (ps[idx] - c) * move;
        }
        move *= 0.5;
    }
    return c;
}

inline Polygon convex_hull(Polygon ps)
{
    std::sort(ps.begin(), ps.end(), point_less);
    ps.erase(std::unique(ps.begin(), ps.end(), [](Point a, Point b) { return eq(a, b); }), ps.end());

    const int n = static_cast<int>(ps.size());
    if (n <= 1)
        return ps;

    Polygon ch(2 * n);
    int k = 0;
    for (int i = 0; i < n; ++i)
    {
        while (k >= 2 && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0)
            --k;
        ch[k++] = ps[i];
    }
    for (int i = n - 2, t = k + 1; i >= 0; --i)
    {
        while (k >= t && ccw(ch[k - 2], ch[k - 1], ps[i]) <= 0)
            --k;
        ch[k++] = ps[i];
    }
    ch.resize(k - 1);
    return ch;
}

inline bool is_ccw_convex(const Polygon &ps)
{
    const int n = static_cast<int>(ps.size());
    for (int i = 0; i < n; ++i)
    {
        if (ccw(ps[i], ps[(i + 1) % n], ps[(i + 2) % n]) == -1)
            return false;
    }
    return true;
}

inline Containment in_convex(Point p, const Polygon &ps)
{
    const int n = static_cast<int>(ps.size());
    int dir = 0;
    for (int i = 0; i < n; ++i)
    {
        const int cur = ccw(ps[i], ps[(i + 1) % n], p);
        if (cur == 0)
            return ON;
        if (dir == 0)
            dir = cur;
        else if (cur != dir)
            return OUT;
    }
    return IN;
}

inline Containment in_ccw_convex(const Polygon &ps, Point p)
{
    const int n = static_cast<int>(ps.size());
    const Point g = (ps[0] + ps[n / 3] + ps[(2 * n) / 3]) / 3.0;
    if (eq(g, p))
        return IN;

    const Point gp = p - g;
    int l = 0, r = n;
    while (l + 1 < r)
    {
        const int mid = (l + r) / 2;
        const Point gl = ps[l] - g;
        const Point gm = ps[mid] - g;
        if (cross(gl, gm) > 0)
        {
            if (cross(gl, gp) >= 0 && cross(gm, gp) <= 0)
                r = mid;
            else
                l = mid;
        }
        else
        {
            if (cross(gl, gp) <= 0 && cross(gm, gp) >= 0)
                l = mid;
            else
                r = mid;
        }
    }
    r %= n;
    const Real cr = cross(ps[l] - p, ps[r] - p);
    if (eq(cr, 0.0))
        return ON;
    return cr < 0 ? OUT : IN;
}

inline Containment in_polygon(const Polygon &ps, Point p)
{
    const int n = static_cast<int>(ps.size());
    bool in = false;
    for (int i = 0; i < n; ++i)
    {
        Point a = ps[i] - p;
        Point b = ps[(i + 1) % n] - p;
        if (eq(cross(a, b), 0.0) && dot(a, b) <= EPS)
            return ON;
        if (a.imag() > b.imag())
            std::swap(a, b);
        if (a.imag() < EPS && EPS < b.imag() && cross(a, b) > EPS)
            in = !in;
    }
    return in ? IN : OUT;
}

inline Polygon convex_cut(const Polygon &ps, Point a1, Point a2)
{
    const int n = static_cast<int>(ps.size());
    Polygon res;
    for (int i = 0; i < n; ++i)
    {
        const Point cur = ps[i];
        const Point nxt = ps[(i + 1) % n];
        const int ccw_cur = ccw(a1, a2, cur);
        const int ccw_nxt = ccw(a1, a2, nxt);
        if (ccw_cur != -1)
            res.push_back(cur);
        if (ccw_cur * ccw_nxt == -1)
            res.push_back(crosspoint_ll(a1, a2, cur, nxt));
    }
    return res;
}

inline std::pair<int, int> convex_diameter(const Polygon &ps)
{
    const int n = static_cast<int>(ps.size());
    int i = static_cast<int>(std::min_element(ps.begin(), ps.end(), point_less) - ps.begin());
    int j = static_cast<int>(std::max_element(ps.begin(), ps.end(), point_less) - ps.begin());

    std::pair<int, int> best{i, j};
    Real best_dist = 0.0;
    for (int step = 0; step < 2 * n; ++step)
    {
        const Real cur = norm2(ps[i] - ps[j]);
        if (cur > best_dist)
        {
            best_dist = cur;
            best = {i, j};
        }
        if (cross(ps[(i + 1) % n] - ps[i], ps[(j + 1) % n] - ps[j]) >= 0)
            j = (j + 1) % n;
        else
            i = (i + 1) % n;
    }
    return best;
}

inline Real area(const Polygon &ps)
{
    Real res = 0.0;
    for (int i = 0; i < static_cast<int>(ps.size()); ++i)
        res += cross(ps[i], ps[(i + 1) % ps.size()]);
    return res * 0.5;
}

inline Point centroid(const Polygon &ps)
{
    Point c(0.0, 0.0);
    Real acc = 0.0;
    for (int i = 0; i < static_cast<int>(ps.size()); ++i)
    {
        const Real a = cross(ps[i], ps[(i + 1) % ps.size()]);
        acc += a;
        c += (ps[i] + ps[(i + 1) % ps.size()]) * a;
    }
    return c / (acc * 3.0);
}

inline Polygon voronoi_cell(Point p, const Polygon &sites, const Polygon &outer)
{
    Polygon cell = outer;
    for (Point q : sites)
    {
        if (eq(p, q))
            continue;
        const Point mid = (p + q) * 0.5;
        cell = convex_cut(cell, mid, mid + rotate90(q - p));
    }
    return cell;
}

inline Graph segment_arrangement(const std::vector<Segment> &segs, Polygon &ps)
{
    const int n = static_cast<int>(segs.size());
    for (int i = 0; i < n; ++i)
    {
        ps.push_back(segs[i].first);
        ps.push_back(segs[i].second);
        for (int j = 0; j < i; ++j)
        {
            if (is_intersect_ss(segs[i].first, segs[i].second, segs[j].first, segs[j].second))
                ps.push_back(crosspoint_ll(segs[i].first, segs[i].second, segs[j].first, segs[j].second));
        }
    }

    std::sort(ps.begin(), ps.end(), point_less);
    ps.erase(std::unique(ps.begin(), ps.end(), [](Point a, Point b) { return eq(a, b); }), ps.end());

    Graph gr(static_cast<int>(ps.size()));
    std::vector<std::pair<Real, int>> order;
    for (int i = 0; i < n; ++i)
    {
        order.clear();
        for (int j = 0; j < static_cast<int>(ps.size()); ++j)
        {
            if (is_intersect_sp(segs[i].first, segs[i].second, ps[j]))
                order.push_back({norm2(segs[i].first - ps[j]), j});
        }
        std::sort(order.begin(), order.end());
        for (int j = 0; j + 1 < static_cast<int>(order.size()); ++j)
        {
            const int a = order[j].second;
            const int b = order[j + 1].second;
            gr.add_edge(a, b, std::abs(ps[a] - ps[b]));
        }
    }
    return gr;
}

inline Graph visibility_graph(const Polygon &ps, const std::vector<Polygon> &objs)
{
    const int n = static_cast<int>(ps.size());
    Graph gr(n);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            const Point a = ps[i];
            const Point b = ps[j];
            if (eq(a, b))
                continue;

            bool ok = true;
            for (const auto &obj : objs)
            {
                const Containment sa = in_convex(a, obj);
                const Containment sb = in_convex(b, obj);
                if (((sa ^ sb) & 1) || ((sa != IN || sb != IN) && in_convex((a + b) * 0.5, obj) == IN))
                {
                    ok = false;
                    break;
                }
                for (int k = 0; k < static_cast<int>(obj.size()); ++k)
                {
                    const Point c = obj[k];
                    const Point d = obj[(k + 1) % obj.size()];
                    if (is_intersect_ss(a, b, c, d) && !is_intersect_sp(c, d, a) && !is_intersect_sp(c, d, b))
                    {
                        ok = false;
                        break;
                    }
                }
                if (!ok)
                    break;
            }
            if (ok)
                gr.add_edge(i, j, std::abs(a - b));
        }
    }
    return gr;
}

inline std::vector<Segment> merge_segments(std::vector<Segment> segs)
{
    int n = static_cast<int>(segs.size());
    for (int i = 0; i < n; ++i)
    {
        if (point_less(segs[i].second, segs[i].first))
            std::swap(segs[i].first, segs[i].second);
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            Segment &l1 = segs[i];
            Segment &l2 = segs[j];
            if (eq(cross(l1.second - l1.first, l2.second - l2.first), 0.0) &&
                is_intersect_lp(l1.first, l1.second, l2.first) &&
                ccw(l1.first, l1.second, l2.second) != +2 &&
                ccw(l2.first, l2.second, l1.second) != +2)
            {
                segs[j] = {point_less(l1.first, l2.first) ? l1.first : l2.first,
                           point_less(l1.second, l2.second) ? l2.second : l1.second};
                segs[i--] = segs[--n];
                break;
            }
        }
    }
    segs.resize(n);
    return segs;
}

} // namespace geometry
