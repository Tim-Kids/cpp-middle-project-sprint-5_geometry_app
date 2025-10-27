#pragma once

#include "geometry.hpp"
#include "queries.hpp"

#include <print>
#include <random>
#include <ranges>
#include <utility>
#include <vector>
#include <algorithm>

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

std::vector<std::pair<Shape, Shape>> FindAllCollisions(ReplaceMe shapes) {
    std::vector<std::pair<Shape, Shape>> collisions;

    /*
     * Используйте библиотеку ranges, чтобы найти все коллизии между фигурами методом BoundingBoxesOverlap
     *
     * Также используйте наиболее эффективный метод добавления объектов в collisions
     */

    return collisions;
}

std::optional<size_t> FindHighestShape(ReplaceMe shapes) {

    /*
     * Используйте библиотеку ranges, чтобы найти самую высокую фигуру
     *
     * Важно: использование ручной итерации по фигурам не разрешается
     */

    return std::nullopt;
}

} // namespace geometry::utils
