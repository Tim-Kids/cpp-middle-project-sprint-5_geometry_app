#pragma once

#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"

#include <span>
#include <ranges>
#include <print>

using namespace geometry;

namespace rs = std::ranges;
namespace rv = std::ranges::views;

inline void PrintAllIntersections(const Shape& base, std::span<const Shape> others) {
    using namespace intersections;

    std::println("\n=== Intersections ===");

    auto filtered = others | rv::enumerate | rv::filter([&](auto&& pair_of_shapes) {
        auto&& [_, s] = pair_of_shapes;
        return &s != &base;
    });

    try {
        for(auto&& [idx, s]: filtered) {
            if(auto ip = GetIntersectPoint(base, s); ip) {
                std::println("Пересечение найдено в точке {} между фигурой #{} и базовой", *ip, idx);
            }
            else {
                std::println("Фигуры базовая и #{} не пересекаются", idx);
            }
        }
    }
    catch(const std::exception& e) {
        std::println("{}", e.what());
    }
}

inline void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    auto first_five = shapes | rv::enumerate | rv::take(5);

    rs::for_each(first_five, [&](auto&& pair_of_shapes) {
        auto [idx, shape] = pair_of_shapes;
        double dist       = queries::DistanceToPoint(shape, p);
        std::println("Расстояние от точки {} до фигуры #{} равно {:.3f}", p, idx, dist);
    });
}

inline void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    // Найти все пересечения между фигурами методом Bounding Box.
    auto collisions = utils::FindAllCollisions(shapes);
    if(!collisions.empty()) {
        std::println("\nОбнаружены коллизии (пересечения Bounding Box):");
        std::ranges::for_each(collisions, [](auto&& pair_of_shapes) {
            const auto& [s1, s2] = pair_of_shapes;
            std::println("  {} пересекается с {}", s1, s2);
        });
    }
    else {
        std::println("\nПересечений Bounding Box не найдено.");
    }

    // Найти самую высокую фигуру.
    if(auto highest_idx = utils::FindHighestShape(shapes)) {
        std::println("\nСамая высокая фигура: #{} (высота = {:.2f})",
                     *highest_idx, queries::GetHeight(shapes[*highest_idx]));
    }
    else {
        std::println("\nНе удалось определить самую высокую фигуру (возможно, список пуст).");
    }

    // Вывести расстояние между любыми двумя фигурами, поддерживающими данную функциональность.
    std::println("\nПоддерживаемые расстояния между фигурами:");
    for(auto [i1, s1]: shapes | rv::enumerate) {
        for(auto [i2, s2]: shapes | rv::enumerate | rv::drop(i1 + 1)) {
            if(auto dist = queries::DistanceBetweenShapes(s1, s2)) {
                std::println("  Расстояние между фигурами #{} и #{} = {:.3f}", i1, i2, *dist);
            }
        }
    }
}

inline void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    // Считаем высоты всех фигур.
    auto indexed_heights = shapes | rv::enumerate | rv::transform([](auto&& id_to_shape) {
        auto [i, shape] = id_to_shape;
        return std::pair{i, queries::GetHeight(shape)};
    });

    // Отбираем 3 шт с высотой выше 50.0.
    auto high_shapes = indexed_heights | rv::filter([](auto&& p) {
                           return p.second > 50.0;
                       })
                       | rv::take(3);

    std::println("\nФигуры, находящиеся выше 50.0:");
    std::ranges::for_each(high_shapes, [&](auto&& p) {
        std::println("  Фигура #{} имеет высоту {:.2f}", p.first, p.second);
    });

    auto indexex_heights_real = indexed_heights | rs::to<std::vector>();
    auto min_it               = std::ranges::min_element(
        indexex_heights_real,
        [](auto&& a, auto&& b) {
            return a.second < b.second;
        });

    auto max_it = std::ranges::max_element(
        indexex_heights_real,
        [](auto&& a, auto&& b) {
            return a.second < b.second;
        });

    if(min_it != std::ranges::end(indexex_heights_real) && max_it != std::ranges::end(indexex_heights_real)) {
        std::println("\nМинимальная высота у фигуры #{} = {:.2f}", min_it->first, min_it->second);
        std::println("Максимальная высота у фигуры #{} = {:.2f}", max_it->first, max_it->second);
    }
    else {
        std::println("\nНе удалось определить минимальные и максимальные фигуры (возможно, список пуст).");
    }
}
