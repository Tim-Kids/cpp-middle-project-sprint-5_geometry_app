#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"
#include "analysis_utils.hpp"

#include <algorithm>
#include <print>
#include <ranges>

using namespace geometry;

namespace rs = std::ranges;
namespace rv = std::ranges::views;

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
    visualization::Draw(shapes);

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
        shapes.emplace_back(std::move(hull_poly));
        visualization::Draw(shapes);
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
            visualization::Draw(
                std::span<triangulation::DelaunayTriangle>((*tri_res).begin(), (*tri_res).end()));
        }
        else {
            std::println("Delaunay failed: error code {}", static_cast<int>(tri_res.error()));
        }
    }
    return 0;
}
