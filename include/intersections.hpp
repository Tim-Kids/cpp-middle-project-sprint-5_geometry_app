#pragma once

#include "geometry.hpp"

#include <cmath>
#include <optional>

using namespace std::literals;

namespace geometry::intersections {

class IntersectionVisitor {
    public:
    // --- Line & Line (отрезки) ---
    std::optional<Point2D> operator()(const Line& l1, const Line& l2) const {
        const Point2D p = l1.start;
        const Point2D r = l1.end - l1.start;
        const Point2D q = l2.start;
        const Point2D s = l2.end - l2.start;

        const double rxs   = r.Cross(s);
        const double q_pxr = (q - p).Cross(r);

        // Параллельны.
        if(std::abs(rxs) < 1e-12) {
            // Коллинеарны — множество пересечений/совпадение отрезков.
            // В рамках текущей спецификации вернём "нет единственной точки".
            return std::nullopt;
        }

        const double t = (q - p).Cross(s) / rxs;
        const double u = (q - p).Cross(r) / rxs;

        if(t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
            // Точка пересечения внутри обоих отрезков.
            return p + r * t;
        }
        return std::nullopt;
    }

    // --- Circle & Circle ---
    std::optional<Point2D> operator()(const Circle& c0, const Circle& c1) const {
        const Point2D p0 = c0.center_p;
        const Point2D p1 = c1.center_p;
        const double r0  = c0.radius;
        const double r1  = c1.radius;

        const double dx = p1.x - p0.x;
        const double dy = p1.y - p0.y;
        const double d  = std::hypot(dx, dy);

        // Нет пересечений или бесконечно много (совпадающие окружности).
        if(d < 1e-12 || d > r0 + r1 || d < std::abs(r0 - r1)) {
            return std::nullopt;
        }

        // a — расстояние от p0 до точки p2 на линии центров.
        const double a    = (r0 * r0 - r1 * r1 + d * d) / (2.0 * d);
        const double h_sq = r0 * r0 - a * a;
        if(h_sq < 0.0) {
            return std::nullopt; // Из-за численной ошибки.
        }
        const double h = std::sqrt(std::max(0.0, h_sq));

        // p2 — проекция точки(точек) пересечения на линию центров.
        const Point2D p2{p0.x + a * dx / d, p0.y + a * dy / d};

        // Одна точка (касание).
        if(h < 1e-12) {
            return p2;
        }

        // Две точки: вернём детерминированно "левую-нижнюю" (по x, затем по y).
        const Point2D i1{p2.x + h * (-dy / d), p2.y + h * (dx / d)};
        const Point2D i2{p2.x - h * (-dy / d), p2.y - h * (dx / d)};
        return (i1.x < i2.x || (std::abs(i1.x - i2.x) < 1e-12 && i1.y <= i2.y)) ? i1 : i2;
    }

    // --- Все прочие комбинации: нет поддержки (по текущей версии файла) ---
    template<class T, class U>
    std::optional<Point2D> operator()(const T&, const U&) const {
        throw std::logic_error(
            "Пересечение фигур не поддерживается! Фигуры: "s + typeid(T).name() + ", " + typeid(U).name());
    }
};

inline std::optional<Point2D> GetIntersectPoint(const Shape& shape1, const Shape& shape2) {
    return std::visit(IntersectionVisitor{}, shape1, shape2);
}

} // namespace geometry::intersections
