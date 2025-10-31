#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "analysis_utils.hpp"

#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <iostream>

using namespace geometry;
using namespace geometry::utils;

// Helper: capture std::println output
static std::string CaptureOutput(const std::function<void()>& func) {
    testing::internal::CaptureStdout();
    func();
    return testing::internal::GetCapturedStdout();
}

TEST(AnalysisUtils, PrintAllIntersections_Basic) {
    Line l1{{0, 0}, {10, 0}};
    Line l2{{5, -5}, {5, 5}};      // Есть пересечение с l1
    Circle c{{20, 20}, 5};         // Нет пересечения с l1.
    std::vector<Shape> shapes = {l1, l2, c};

    auto output = CaptureOutput([&]() {
        PrintAllIntersections(shapes[0], shapes);
    });

    EXPECT_TRUE(output.find("Пересечение найдено") != std::string::npos)
        << "Should detect intersection between l1 and l2";
    EXPECT_TRUE(output.find("не пересекаются") == std::string::npos)
        << "Should report no intersection for unrelated shape";
}

TEST(AnalysisUtils, PrintDistancesFromPointToShapes_NoCrash) {
    std::vector<Shape> shapes = {
        Line{{0, 0}, {1, 0}},
        Circle{{0, 0}, 5},
        Rectangle{{0, 0}, 2, 2},
        Triangle{{0, 0}, {1, 0}, {0, 1}},
        RegularPolygon{{0, 0}, 3.0, 6}
    };

    auto output = CaptureOutput([&]() {
        PrintDistancesFromPointToShapes(Point2D{2.0, 2.0}, shapes);
    });

    EXPECT_NE(output.find("Расстояние"), std::string::npos)
        << "Output should contain distances";
}

TEST(AnalysisUtils, PerformShapeAnalysis_NoCrashAndContainsKeywords) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 2, 2},
        Rectangle{{1, 1}, 2, 2},   // Пересекается с предыдущим.
        Circle{{10, 10}, 1}
    };

    auto output = CaptureOutput([&]() {
        PerformShapeAnalysis(shapes);
    });

    EXPECT_NE(output.find("Shape Analysis"), std::string::npos);
    EXPECT_NE(output.find("Bounding Box"), std::string::npos);
    EXPECT_NE(output.find("высота"), std::string::npos);
}

TEST(AnalysisUtils, PerformExtraShapeAnalysis_MinMaxPrinted) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 2, 2},
        Circle{{0, 100}, 10},
        Triangle{{0, 0}, {1, 1}, {2, 0}},
        Line{{-5, 50}, {5, 50}}
    };

    auto output = CaptureOutput([&]() {
        PerformExtraShapeAnalysis(shapes);
    });

    EXPECT_NE(output.find("Минимальная высота"), std::string::npos);
    EXPECT_NE(output.find("Максимальная высота"), std::string::npos);
}

TEST(AnalysisUtils, StressTest_RandomShapesPerformance) {
    ShapeGenerator generator(-100.0, 100.0, 5.0, 25.0);

    // Генерируем 100 фигур.
    constexpr size_t kShapeCount = 100;
    std::vector<Shape> shapes = generator.GenerateShapes(kShapeCount);

    ASSERT_EQ(shapes.size(), kShapeCount);

    // Профилируем.
    auto start = std::chrono::steady_clock::now();

    EXPECT_NO_THROW({
        PrintAllIntersections(shapes[0], shapes);
        PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);
        PerformShapeAnalysis(shapes);
        PerformExtraShapeAnalysis(shapes);
    });

    auto end = std::chrono::steady_clock::now();
    auto duration_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Проверяем,что укалдываемся в тайминг - цифра взята, исходя из аппаратных возможностей.
    EXPECT_LT(duration_ms, 500)
        << "Analysis utilities took too long to process random shapes (" << duration_ms << " ms)";
}
