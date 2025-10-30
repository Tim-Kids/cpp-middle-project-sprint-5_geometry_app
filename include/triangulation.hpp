#pragma once

#include "geometry.hpp"

#include <algorithm>
#include <format>
#include <set>
#include <vector>

namespace geometry::triangulation {

struct DelaunayTriangle {
    Point2D a, b, c;

    DelaunayTriangle(Point2D a, Point2D b, Point2D c) :
        a(a),
        b(b),
        c(c) {
    }

    [[nodiscard]] bool ContainsPoint(const Point2D& p) const noexcept {
        Point2D center = Circumcenter();
        double radius  = Circumradius();
        return center.DistanceTo(p) <= radius + 1e-10;
    }

    [[nodiscard]] Point2D Circumcenter() const {
        double d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
        if(std::abs(d) < 1e-10) {
            return {(a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3};
        }

        double ux = ((a.x * a.x + a.y * a.y) * (b.y - c.y) + (b.x * b.x + b.y * b.y) * (c.y - a.y) +
                     (c.x * c.x + c.y * c.y) * (a.y - b.y)) /
                    d;

        double uy = ((a.x * a.x + a.y * a.y) * (c.x - b.x) + (b.x * b.x + b.y * b.y) * (a.x - c.x) +
                     (c.x * c.x + c.y * c.y) * (b.x - a.x)) /
                    d;

        return {ux, uy};
    }

    [[nodiscard]] double Circumradius() const noexcept {
        Point2D center = Circumcenter();
        return center.DistanceTo(a);
    }

    [[nodiscard]] bool SharesEdge(const DelaunayTriangle& other) const {
        std::vector<Point2D> this_points  = {a, b, c};
        std::vector<Point2D> other_points = {other.a, other.b, other.c};

        int shared_count = 0;
        for(const Point2D& p1: this_points) {
            for(const Point2D& p2: other_points) {
                if(std::abs(p1.x - p2.x) < 1e-10 && std::abs(p1.y - p2.y) < 1e-10) {
                    shared_count++;
                    break;
                }
            }
        }

        return shared_count == 2;
    }

    [[nodiscard]] std::vector<Point2D> vertices() const noexcept {
        return {a, b, c};
    }
};

struct Edge {
    Point2D p1, p2;

    Edge(Point2D p1, Point2D p2) :
        p1(p1),
        p2(p2) {
        if(p1.x > p2.x || (p1.x == p2.x && p1.y > p2.y)) {
            std::swap(this->p1, this->p2);
        }
    }

    bool operator<(const Edge& other) const {
        if(std::abs(p1.x - other.p1.x) > 1e-10) {
            return p1.x < other.p1.x;
        }
        if(std::abs(p1.y - other.p1.y) > 1e-10) {
            return p1.y < other.p1.y;
        }
        if(std::abs(p2.x - other.p2.x) > 1e-10) {
            return p2.x < other.p2.x;
        }
        return p2.y < other.p2.y;
    }

    bool operator==(const Edge& other) const {
        return std::abs(p1.x - other.p1.x) < 1e-10 && std::abs(p1.y - other.p1.y) < 1e-10 &&
               std::abs(p2.x - other.p2.x) < 1e-10 && std::abs(p2.y - other.p2.y) < 1e-10;
    }
};

inline GeometryResult<std::vector<DelaunayTriangle>> DelaunayTriangulation(std::span<const Point2D> points) {
    // Триангуляция Делоне алгоритмом Боуэра-Ватсона
    if(points.size() < 3) {
        return std::unexpected(GeometryError::InsufficientPoints);
    }

    // 1. Создаём список для хранения текущей триангуляции и добавляем в него "Супер-треугольник",
    // содержащий внутри себя все точки.
    double min_x = points[0].x, max_x = points[0].x;
    double min_y = points[0].y, max_y = points[0].y;

    for(const auto& p: points) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    const double dx    = max_x - min_x;
    const double dy    = max_y - min_y;
    const double delta = std::max(dx, dy) * 10.0;

    Point2D super1{min_x - delta, min_y - delta};
    Point2D super2{min_x + 0.5 * dx, max_y + delta};
    Point2D super3{max_x + delta, min_y - delta};

    std::vector<DelaunayTriangle> triangulation;
    triangulation.emplace_back(super1, super2, super3);

    // 2. Алгоритм Bowyer-Watson.
    for(const auto& p: points) {
        //  Находятся все "плохие" треугольники (из текущей триангуляции), в чьи описанные окружности входит эта
        //  точка(ContainsPoint); "плохими" называются треугольники, нарушающие условие Делоне(внутри окружности не
        //  должно быть других точек)
        std::vector<size_t> bad_idx;
        bad_idx.reserve(triangulation.size());
        for(size_t i = 0; i < triangulation.size(); ++i) {
            if(triangulation[i].ContainsPoint(p)) {
                bad_idx.push_back(i);
            }
        }

        // Для всех рёбер этих треугольников формируется множество polygon, причём:
        // - Если ребро ещё не встречалось - оно добавляется в polygon.
        // - Если ребро встречается второй раз - оно удаляется из polygon.
        std::multiset<Edge> edge_buffer;
        for(size_t idx: bad_idx) {
            const auto& t                   = triangulation[idx];
            const std::array<Edge, 3> edges = {
                Edge{t.a, t.b},
                Edge{t.b, t.c},
                Edge{t.c, t.a}
            };
            for(const auto& e: edges) {
                auto it = edge_buffer.find(e);
                if(it == edge_buffer.end()) {
                    edge_buffer.insert(e);
                }
                else {
                    edge_buffer.erase(it);
                }
            }
        }

        std::erase_if(triangulation, [&](const DelaunayTriangle& t) {
            return t.ContainsPoint(p);
        });

        // Для каждой границы "дырки" (polygonal hole) создаются новые треугольники с новой точкой: { ТочкаРебра1, ТочкаРебра2, НоваяТочка }.
        for(const auto& edge: edge_buffer) {
            triangulation.emplace_back(edge.p1, edge.p2, p);
        }
    }

    // 3. Удаляем все треугольники, включающие вершины супер-треугольника.
    std::erase_if(triangulation, [&](const DelaunayTriangle& t) {
        const auto verts = t.vertices();
        auto has_vertex  = [&](const Point2D& v) {
            auto close = [](const Point2D& p1, const Point2D& p2) {
                return std::abs(p1.x - p2.x) < 1e-10 && std::abs(p1.y - p2.y) < 1e-10;
            };
            return close(v, super1) || close(v, super2) || close(v, super3);
        };
        return std::ranges::any_of(verts, has_vertex);
    });

    if(triangulation.empty()) {
        return std::unexpected(GeometryError::DegenrateCase);
    }

    return triangulation;
}
} // namespace geometry::triangulation

template<>
struct std::formatter<geometry::triangulation::DelaunayTriangle> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::triangulation::DelaunayTriangle& t, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "DelaunayTriangle({}, {}, {})", t.a, t.b, t.c);
    }
};
