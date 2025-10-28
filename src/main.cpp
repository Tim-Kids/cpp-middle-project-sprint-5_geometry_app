#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>

using namespace geometry;

namespace rs = std::ranges;
namespace rv = std::ranges::views;

void PrintAllIntersections(const Shape& base, std::span<const Shape> others) {
    using namespace intersections;


    std::println("\n=== Intersections ===");

    auto filtered = others | rv::enumerate | rv::filter([&](auto&& pair) {
             auto&& [i, s] = pair;
             return &s != &base;
         });

    try {
        for(auto&& [idx, s]: filtered) {
            if(auto ip = GetIntersectPoint(base, s); ip.has_value()) {
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

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    auto first_five = shapes | rv::enumerate | rv::take(5);

    rs::for_each(first_five, [&](auto&& pair) {
              auto [idx, shape] = pair;
              double dist = queries::DistanceToPoint(shape, p);
              std::println("Расстояние от точки {} до фигуры #{} равно {:.3f}", p, idx, dist);
          });
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    // Найти все пересечения между фигурами методом Bounding Box.
    auto collisions = utils::FindAllCollisions(shapes);
    if (!collisions.empty()) {
        std::println("\nОбнаружены коллизии (пересечения Bounding Box):");
        std::ranges::for_each(collisions, [](auto&& pair) {
            const auto& [s1, s2] = pair;
            std::println("  {} пересекается с {}", s1, s2);
        });
    } else {
        std::println("\nПересечений Bounding Box не найдено.");
    }

    // Найти самую высокую фигуру.
    if (auto highest_idx = utils::FindHighestShape(shapes)) {
        std::println("\nСамая высокая фигура: #{} (высота = {:.2f})",
                     *highest_idx, queries::GetHeight(shapes[*highest_idx]));
    } else {
        std::println("\nНе удалось определить самую высокую фигуру (возможно, список пуст).");
    }

    // Вывести расстояние между любыми двумя фигурами, поддерживающими данную функциональность.
    std::println("\nПоддерживаемые расстояния между фигурами:");
    for (auto [i1, s1] : shapes | rv::enumerate) {
        for (auto [i2, s2] : shapes | rv::enumerate | rv::drop(i1 + 1)) {
            if (auto dist = queries::DistanceBetweenShapes(s1, s2)) {
                std::println("  Расстояние между фигурами #{} и #{} = {:.3f}", i1, i2, *dist);
            }
        }
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    // Считаем высоты всех фигур.
    auto indexed_heights = shapes | rv::enumerate | rv::transform([](auto&& id_to_shape) {
                                         auto [i, shape] = id_to_shape;
                                         return std::pair{i, queries::GetHeight(shape)};
                                     });

    // Отбираем 3 шт с высотой выше 50.0.
    auto high_shapes = indexed_heights | rv::filter([](auto&& p) { return p.second > 50.0; })
                                       | rv::take(3);

    std::println("\nФигуры, находящиеся выше 50.0:");
    std::ranges::for_each(high_shapes, [&](auto&& p) {
        std::println("  Фигура #{} имеет высоту {:.2f}", p.first, p.second);
    });

    auto indexex_heights_real = indexed_heights | rs::to<std::vector>();
    auto min_it = std::ranges::min_element(
        indexex_heights_real,
        [](auto&& a, auto&& b) { return a.second < b.second; });

    auto max_it = std::ranges::max_element(
        indexex_heights_real,
        [](auto&& a, auto&& b) { return a.second < b.second; });

    if(min_it != std::ranges::end(indexex_heights_real) && max_it != std::ranges::end(indexex_heights_real)) {
        std::println("\nМинимальная высота у фигуры #{} = {:.2f}", min_it->first, min_it->second);
        std::println("Максимальная высота у фигуры #{} = {:.2f}", max_it->first, max_it->second);
    }
    else {
        std::println("\nНе удалось определить минимальные и максимальные фигуры (возможно, список пуст).");
    }
}

int main() {
    utils::ShapeGenerator generator(-50.0, 50.0, 5.0, 25.0);

    // 1. После реализации всех фигур, замените GenerateTriangles на GenerateShapes
    std::vector<Shape> shapes = generator.GenerateShapes(15);

    std::println("Generated {} random shapes", shapes.size());

    // 2. Выведите индекс каждой фигуры и её высоту
    for(auto [id, shape]: std::views::enumerate(shapes)) {
        std::println("Shape #{:02d}: height = {:.2f}", id, queries::GetHeight(shape));
    }

    // 3. Вызываем разработанные функции
    PrintAllIntersections(shapes[0], shapes);
    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);
    PerformShapeAnalysis(shapes);
    PerformExtraShapeAnalysis(shapes);

    // 4. Рисуем все фигуры. Требуется поддержка gnuplot.
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    // visualization::Draw(shapes);

    // 5. Формируем список из вершин всех фигур
    std::vector<Point2D> points;
    for(const auto& sh: shapes) {
        std::visit([&](const auto& s) {
            auto verts = s.Vertices();
            points.insert(points.end(), verts.begin(), verts.end());
        }, sh);
    }

    // 6. Удалим дубликаты точек - на всякий случай.
    constexpr double eps = 1e-9;
    rs::sort(points, {}, [](const Point2D& p) {
        return std::pair{p.x, p.y};
    });
    auto last = std::unique(points.begin(), points.end(),
                            [&](const Point2D& a, const Point2D& b) {
                                return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps;
                            });
    points.erase(last, points.end());

    // 7. Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема.
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes. Рисуем все фигуры.
    if(auto hull_res = convex_hull::GrahamScan(points)) {
        const auto& hull_pts = *hull_res;
        std::println("\nConvex hull has {} vertices", hull_pts.size());
        for(const auto& p: hull_res.value()) {
            std::println("\t{}", p);
        }

        // Добавим оболочку как Polygon и перерисуем
        Polygon hull_poly(hull_pts);
        shapes.push_back(hull_poly);
        // visualization::Draw(shapes);
    }
    else {
        std::println("Convex hull failed: error code {}", static_cast<int>(hull_res.error()));
    }

    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    {
        // Используйте список точек points или свой, чтобы выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        // После успешного завершения алгоритма - выведите результат для проверки используя visualization::Draw.
        std::vector<Point2D> pts = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};
        if(auto tri_res = triangulation::DelaunayTriangulation(pts)) {
            std::println("\nDelaunay produced {} triangles", (*tri_res).size());
            // visualization::Draw(
                // std::span<const triangulation::DelaunayTriangle>((*tri_res).begin(), (*tri_res).end()));
        }
        else {
            std::println("Delaunay failed: error code {}", static_cast<int>(tri_res.error()));
        }
    }
    return 0;
}
