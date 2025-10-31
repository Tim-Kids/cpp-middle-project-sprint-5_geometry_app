#pragma once

#include "geometry.hpp"
#include "queries.hpp"

#include <span>
#include <random>
#include <ranges>
#include <utility>
#include <vector>
#include <algorithm>

namespace rs = std::ranges;
namespace rv = std::ranges::views;

namespace geometry::utils {

inline void RemoveDuplicates(std::vector<Point2D>& points) {
    constexpr double eps = 1e-9;
    std::ranges::sort(points, {}, [](const Point2D& p) {
        return std::pair{p.x, p.y};
    });
    auto last = std::unique(points.begin(), points.end(), [&](const Point2D& a, const Point2D& b) {
        return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps;
    });
    points.erase(last, points.end());
}

class ShapeGenerator {
    public:
    explicit ShapeGenerator(double min_coord = -100.0, double max_coord = 100.0, double min_size = 1.0,
                            double max_size  = 20.0) :
        gen_(20),
        coord_dist_(min_coord, max_coord),
        size_dist_(min_size, max_size),
        sides_dist_(3, 12),
        type_dist_(0, 4) {
    }

    Shape GenerateRandomShape() {
        Point2D center{coord_dist_(gen_), coord_dist_(gen_)};
        double size = size_dist_(gen_);

        switch(type_dist_(gen_)) {
            case 0: {
                Point2D end{center.x + size, center.y + size};
                return Line{center, end};
            }
            case 1: {
                Point2D a{center.x, center.y};
                Point2D b{center.x + size, center.y};
                Point2D c{center.x + size / 2, center.y + size};
                return Triangle{a, b, c};
            }
            case 2: {
                return Rectangle{center, size, size * 0.8};
            }
            case 3: {
                int sides = sides_dist_(gen_);
                return RegularPolygon{center, size, sides};
            }
            case 4: {
                return Circle{center, size};
            }
        }
        return Circle{center, size};
    }

    std::vector<Shape> GenerateShapes(size_t count) {
        std::vector<Shape> shapes;
        shapes.reserve(count);

        for(auto _: std::views::iota(0u, count)) {
            shapes.push_back(GenerateRandomShape());
        }

        return shapes;
    }

    std::vector<Shape> GenerateTriangles(size_t count) {
        std::vector<Shape> shapes;
        shapes.reserve(count);

        for(auto _: std::views::iota(0u, count)) {
            Point2D center{coord_dist_(gen_), coord_dist_(gen_)};
            double size = size_dist_(gen_);
            Point2D a{center.x, center.y};
            Point2D b{center.x + size, center.y};
            Point2D c{center.x + size / 2, center.y + size};
            shapes.emplace_back(Triangle{a, b, c});
        }

        return shapes;
    }

    private:
    std::mt19937 gen_;
    std::uniform_real_distribution<double> coord_dist_;
    std::uniform_real_distribution<double> size_dist_;
    std::uniform_int_distribution<int> sides_dist_;
    std::uniform_int_distribution<int> type_dist_;
};

inline std::vector<std::pair<Shape, Shape>> FindAllCollisions(std::span<const Shape> shapes) {
    auto collected_shapes = shapes | rv::enumerate | rv::transform([&](auto&& indexed_left) {
        auto&& [id_left, left_shape] = indexed_left;
        // каждый раз сдвигаем view на один элемент вправо, чтобы повторно не сравнивать предыдущие элементы.
        auto tail_shapes             = shapes | rv::drop(id_left + 1);
        return tail_shapes | rv::filter([&](auto&& right_shape) {
            return queries::BoundingBoxesOverlap(left_shape, right_shape);
        }) | rv::transform([&](auto&& right_shape) {
            return std::pair{left_shape, right_shape};
        });
    }) | rv::join /*укладываем все пары из nested if в один ряд*/ | rs::to<std::vector>();
    return collected_shapes;
}

inline std::optional<size_t> FindHighestShape(std::span<const Shape> shapes) {
    if(shapes.empty()) {
        return std::nullopt;
    }

    auto it = std::ranges::max_element(shapes, {}, [](const Shape& s) {
        return queries::GetHeight(s);
    });

    return static_cast<size_t>(std::distance(shapes.begin(), it));
}

} // namespace geometry::utils
