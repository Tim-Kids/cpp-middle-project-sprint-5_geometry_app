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

namespace rng = std::ranges;
namespace views = std::ranges::views;

void PrintAllIntersections(const Shape &shape, ReplaceMe others) {
    std::println("\n=== Intersections ===");

    /*
     * Используйте ranges чтобы оставить только фигуры,
     * поддерживающие возможность находить пересечения между собой
     *
     * Затем примените монадический интерфейс для обработки результатов:
     *     - Пересечение найдено в точке A между фигурами B и C
     *     - Фигуры B и C не пересекаются
     */
}

void PrintDistancesFromPointToShapes(Point2D p, ReplaceMe shapes) {
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    /*
     * Используйте ranges чтобы выбрать любые 5 фигур из списка.
     * Затем найдите расстояния от заданной точки до всех выбранных фигур.
     * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
     */
}

void PerformShapeAnalysis(ReplaceMe shapes) {
    std::println("\n=== Shape Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Найти все пересечения между фигурами используя метод Bounding Box
     *     - Найти самую высокую фигуру (чья высота наибольшая)
     *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают данную функциональность
     */
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */
}

int main() {
    utils::ShapeGenerator generator(-50.0, 50.0, 5.0, 25.0);

    //
    // После реализации всех фигур, замените GenerateTriangles на GenerateShapes
    //
    std::vector<Shape> shapes = generator.GenerateShapes(15);

    std::println("Generated {} random shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту
    for(auto [i, s]: views::enumerate(shapes)) {
        auto height = std::visit([](const auto& sh) {
            return sh.Height();
        }, s);
        std::println("Shape #{:02d}: height = {:.2f}", i, height);
    }

    //
    // Вызываем разработанные функции
    //
    // PrintAllIntersections(shapes[0], shapes);

    // PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    // PerformShapeAnalysis(shapes);

    // PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    // geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points;

    /* ваш код здесь */
    for(const auto& shape: shapes) {
        std::visit([&](const auto& s) {
            const auto verts = s.Vertices();
            points.insert(points.end(), verts.begin(), verts.end());
        }, shape);
    }

    // [[optional]] Remove duplicates (within a small epsilon).
    utils::RemoveDuplicates(points);

    std::println("\nCollected {} points for convex hull", points.size());

    /*std::println("Unique point count after filtering: {}", points.size());
    for (auto& p : points)
        std::println("  ({:.2f}, {:.2f})", p.x, p.y);*/


    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    /* ваш код здесь */
    if (auto hull_res = convex_hull::GrahamScan(points)) {
        auto& hull_points = *hull_res;
        std::println("Convex hull has {} vertices", hull_points.size());
        // std::println("{:new_line}", hull_points);

        // Create polygon from hull and add to shapes
        Polygon hull_poly(hull_points);
        shapes.emplace_back(std::move(hull_poly));

        std::println("Drawing convex hull together with initial shapes...");
        // geometry::visualization::Draw(shapes); // Needs 'gnuplot' onboard installed.
    } else {
        std::println("Convex hull failed: error code {}", static_cast<int>(hull_res.error()));
    }

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        // std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
    }
    return 0;
}