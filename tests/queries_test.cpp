#include <gtest/gtest.h>

#include "queries.hpp"

using namespace geometry;
using namespace geometry::queries;

TEST(PointToShapeDistance, Line) {
    Line l{{0, 0}, {10, 0}};
    Point2D p{5, 5};
    double d = std::visit(PointToShapeDistanceVisitor{p}, Shape{l});
    EXPECT_NEAR(d, 5.0, 1e-9);
}

TEST(PointToShapeDistance, Circle_InsideAndOutside) {
    Circle c{{0, 0}, 3.0};
    EXPECT_NEAR(std::visit(PointToShapeDistanceVisitor{{0,0}}, Shape{c}), 3.0, 1e-9); // center
    EXPECT_NEAR(std::visit(PointToShapeDistanceVisitor{{3,0}}, Shape{c}), 0.0, 1e-9); // on border
    EXPECT_NEAR(std::visit(PointToShapeDistanceVisitor{{5,0}}, Shape{c}), 2.0, 1e-9); // outside
}

TEST(PointToShapeDistance, Triangle) {
    Triangle t{{0, 0}, {4, 0}, {0, 3}};
    Point2D p{2, 1};
    double d = std::visit(PointToShapeDistanceVisitor{p}, Shape{t});
    EXPECT_NEAR(d, 0.0, 1e-9); // inside
}

TEST(PointToShapeDistance, RectangleOutside) {
    Rectangle r{{0, 0}, 4, 3};
    Point2D p{5, 1.5};
    double d = std::visit(PointToShapeDistanceVisitor{p}, Shape{r});
    EXPECT_NEAR(d, 1.0, 1e-9);
}

TEST(ShapeToShapeDistance, LineLine_NonParallel) {
    Line a{{0, 0}, {3, 0}};
    Line b{{0, 4}, {3, 4}};
    auto d = ShapeToShapeDistanceVisitor{}(a, b);
    ASSERT_TRUE(d.has_value());
    EXPECT_NEAR(*d, 4.0, 1e-9);
}

TEST(ShapeToShapeDistance, CircleCircle_SeparateAndTouch) {
    Circle c1{{0, 0}, 1.0};
    Circle c2{{5, 0}, 1.0};
    auto d = ShapeToShapeDistanceVisitor{}(c1, c2);
    ASSERT_TRUE(d.has_value());
    EXPECT_NEAR(*d, 3.0, 1e-9);

    Circle c3{{2, 0}, 1.0};
    auto d2 = ShapeToShapeDistanceVisitor{}(c1, c3);
    EXPECT_NEAR(*d2, 0.0, 1e-9);
}

TEST(ShapeToShapeDistance, AnyPoint_Distance) {
    Circle c{{0, 0}, 2};
    Point2D p{5, 0};
    auto d = ShapeToShapeDistanceVisitor{}(Shape{c}, p);
    ASSERT_TRUE(d.has_value());
    EXPECT_NEAR(*d, 3.0, 1e-9);
}

TEST(ShapeToShapeDistance, Unsupported_ReturnsNullopt) {
    Rectangle r{{0, 0}, 1, 1};
    Triangle t{{0, 0}, {1, 0}, {0, 1}};
    auto d = ShapeToShapeDistanceVisitor{}(r, t);
    EXPECT_FALSE(d.has_value());
}

TEST(Helpers, BoundingBoxesOverlapAndGetHeight) {
    Rectangle r1{{0, 0}, 2, 2};
    Rectangle r2{{1, 1}, 2, 2};
    Shape s1{r1}, s2{r2};
    EXPECT_TRUE(BoundingBoxesOverlap(s1,s2));
    EXPECT_NEAR(GetHeight(s1), 2.0, 1e-9);
}

TEST(Helpers, DistanceBetweenShapes_Works) {
    Circle c1{{0, 0}, 1.0};
    Circle c2{{5, 0}, 1.0};
    auto d = DistanceBetweenShapes(Shape{c1}, Shape{c2});
    ASSERT_TRUE(d.has_value());
    EXPECT_NEAR(*d, 3.0, 1e-9);
}
