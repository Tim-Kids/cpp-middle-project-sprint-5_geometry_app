#pragma once

#include "geometry.hpp"

#include <algorithm>
#include <optional>
#include <variant>

namespace geometry::queries {

template<class... Ts>
struct Multilambda: Ts... {
    using Ts::operator()...;
};

inline double DistPointToSegment(const Point2D& p, const Point2D& a, const Point2D& b) {
    const Point2D ab  = b - a;
    const double len2 = ab.Dot(ab);
    if(len2 < 1e-18) {
        return p.DistanceTo(a);
    }
    double t           = ((p - a).Dot(ab)) / len2;
    t                  = std::clamp(t, 0.0, 1.0);
    const Point2D proj = a + ab * t;
    return p.DistanceTo(proj);
}

inline bool PointInTriangle(const Point2D& p, const Triangle& t) {
    auto sgn = [](Point2D p1, Point2D p2, Point2D p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };
    bool b1 = sgn(p, t.a, t.b) < 0.0;
    bool b2 = sgn(p, t.b, t.c) < 0.0;
    bool b3 = sgn(p, t.c, t.a) < 0.0;
    return (b1 == b2) && (b2 == b3);
}

inline bool PointInRectangle(const Point2D& p, const Rectangle& r) {
    return p.x >= r.bottom_left.x &&
           p.x <= r.bottom_left.x + r.width &&
           p.y >= r.bottom_left.y &&
           p.y <= r.bottom_left.y + r.height;
}

inline bool PointInPolygonRayCast(const Point2D& p, std::span<const Point2D> verts) {
    bool inside = false;
    if(verts.size() < 3) {
        return false;
    }
    for(size_t i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
        const auto& pi       = verts[i];
        const auto& pj       = verts[j];
        const bool intersect = ((pi.y > p.y) != (pj.y > p.y)) &&
                               (p.x < (pj.x - pi.x) * (p.y - pi.y) / ((pj.y - pi.y) == 0 ? 1e-18 : (pj.y - pi.y)) + pi.
                                x);
        if(intersect) {
            inside = !inside;
        }
    }
    return inside;
}

struct PointToShapeDistanceVisitor {
    Point2D point;

    explicit PointToShapeDistanceVisitor(const Point2D& p) :
        point(p) {
    }

    double operator()(const Point2D& q) const {
        return point.DistanceTo(q);
    }

    double operator()(const Line& l) const {
        return DistPointToSegment(point, l.start, l.end);
    }

    double operator()(const Circle& c) const {
        return std::abs(point.DistanceTo(c.center_p) - c.radius);
    }

    double operator()(const Triangle& t) const {
        if(PointInTriangle(point, t)) {
            return 0.0;
        }
        return std::min({DistPointToSegment(point, t.a, t.b),
                         DistPointToSegment(point, t.b, t.c),
                         DistPointToSegment(point, t.c, t.a)});
    }

    double operator()(const Rectangle& r) const {
        if(PointInRectangle(point, r)) {
            return 0.0;
        }
        const auto v = r.Vertices();
        return std::min({DistPointToSegment(point, v[0], v[1]),
                         DistPointToSegment(point, v[1], v[2]),
                         DistPointToSegment(point, v[2], v[3]),
                         DistPointToSegment(point, v[3], v[0])});
    }

    double operator()(const RegularPolygon& rp) const {
        const auto verts = rp.Vertices();
        if(PointInPolygonRayCast(point, verts)) {
            return 0.0;
        }
        double dmin = std::numeric_limits<double>::infinity();
        for(size_t i = 0; i < verts.size(); ++i) {
            const auto& a = verts[i];
            const auto& b = verts[(i + 1) % verts.size()];
            dmin          = std::min(dmin, DistPointToSegment(point, a, b));
        }
        return dmin;
    }

    double operator()(const Polygon& poly) const {
        const auto verts = poly.Vertices();
        if(PointInPolygonRayCast(point, verts)) {
            return 0.0;
        }
        double dmin = std::numeric_limits<double>::infinity();
        for(size_t i = 0; i < verts.size(); ++i) {
            const auto& a = verts[i];
            const auto& b = verts[(i + 1) % verts.size()];
            dmin          = std::min(dmin, DistPointToSegment(point, a, b));
        }
        return dmin;
    }
};

struct ShapeToShapeDistanceVisitor {
    // Any & Point
    std::optional<double> operator()(const Shape& s, const Point2D& p) const {
        return std::visit(PointToShapeDistanceVisitor{p}, s);
    }

    std::optional<double> operator()(const Point2D& p, const Shape& s) const {
        return std::visit(PointToShapeDistanceVisitor{p}, s);
    }

    // Line & Line (расстояние между отрезками)
    std::optional<double> operator()(const Line& a, const Line& b) const {
        // Если пересекаются — 0
        // (простая проверка через проекции/знаки, здесь используем расстояние "точка-отрезок" в обе стороны)
        const double d = std::min({
            DistPointToSegment(a.start, b.start, b.end),
            DistPointToSegment(a.end, b.start, b.end),
            DistPointToSegment(b.start, a.start, a.end),
            DistPointToSegment(b.end, a.start, a.end)
        });
        return d;
    }

    // Circle & Circle
    std::optional<double> operator()(const Circle& c0, const Circle& c1) const {
        const double d   = c0.center_p.DistanceTo(c1.center_p);
        const double gap = d - (c0.radius + c1.radius);
        return std::max(0.0, gap);
    }

    // Все прочее — неподдерживаемо.
    template<class T, class U>
    std::optional<double> operator()(const T&, const U&) const {
        return std::nullopt;
    }
};

inline double DistanceToPoint(const Shape& shape, const Point2D& point) {
    return std::visit(PointToShapeDistanceVisitor{point}, shape);
}

inline BoundingBox GetBoundBox(const Shape& shape) {
    return std::visit([](const auto& s) {
        return s.BoundBox();
    }, shape);
}

inline double GetHeight(const Shape& shape) {
    return std::visit([](const auto& s) {
        return s.Height();
    }, shape);
}

inline bool BoundingBoxesOverlap(const Shape& shape1, const Shape& shape2) {

    /* ваш код здесь */
    const auto bb1 = GetBoundBox(shape1);
    const auto bb2 = GetBoundBox(shape2);
    return bb1.Overlaps(bb2);
}

inline std::optional<double> DistanceBetweenShapes(const Shape& shape1, const Shape& shape2) {

    /* ваш код с ShapeToShapeDistanceVisitor здесь*/

    // Пытаемся применить поддерживаемые комбинации.
    // Line & Line
    if(std::holds_alternative<Line>(shape1) && std::holds_alternative<Line>(shape2)) {
        return ShapeToShapeDistanceVisitor{}(std::get<Line>(shape1), std::get<Line>(shape2));
    }
    // Circle & Circle
    if(std::holds_alternative<Circle>(shape1) && std::holds_alternative<Circle>(shape2)) {
        return ShapeToShapeDistanceVisitor{}(std::get<Circle>(shape1), std::get<Circle>(shape2));
    }

    // Неподдерживаемо.
    return std::nullopt;
}

} // namespace geometry::queries
