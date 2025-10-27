#include <gtest/gtest.h>

#include  "intersections.hpp"

using namespace geometry;
using namespace geometry::intersections;

// ------------------------------------------------------
//  Line–Line
// ------------------------------------------------------

TEST(Intersections, LineLine_IntersectInsideSegments) {
    Line a{{0,0},{4,4}};
    Line b{{0,4},{4,0}};
    auto p = GetIntersectPoint(a, b);
    ASSERT_TRUE(p.has_value());
    EXPECT_NEAR(p->x, 2.0, 1e-9);
    EXPECT_NEAR(p->y, 2.0, 1e-9);
}

TEST(Intersections, LineLine_NoIntersection) {
    Line a{{0,0},{1,0}};
    Line b{{0,1},{1,1}};
    auto p = GetIntersectPoint(a, b);
    EXPECT_FALSE(p.has_value());
}

// ------------------------------------------------------
//  Circle–Circle, exception on unsupported
// ------------------------------------------------------

TEST(Intersections, CircleCircle_TangentTouch) {
    Circle c1{{0,0}, 1.0};
    Circle c2{{2,0}, 1.0};
    auto p = GetIntersectPoint(c1, c2);
    ASSERT_TRUE(p.has_value());
    EXPECT_NEAR(p->x, 1.0, 1e-9);
    EXPECT_NEAR(p->y, 0.0, 1e-9);
}

TEST(Intersections, CircleCircle_TwoPoints_ReturnDeterministicOne) {
    Circle c1{{0,0}, 5.0};
    Circle c2{{6,0}, 5.0};
    auto p = GetIntersectPoint(c1, c2);
    ASSERT_TRUE(p.has_value());
    // One of the two intersection points lies above x-axis.
    EXPECT_NEAR(p->x, 3.0, 1e-9);
    EXPECT_NEAR(std::abs(p->y), 4.0, 1e-9);
}

// ------------------------------------------------------
//  Circle–Circle, exception on unsupported
// ------------------------------------------------------

TEST(Intersections, Unsupported_ThrowsLogicError) {
    Triangle t{{0,0},{1,0},{0,1}};
    Rectangle r{{0,0}, 1.0, 1.0};
    EXPECT_THROW({
        std::visit(IntersectionVisitor{}, Shape{t}, Shape{r});
    }, std::logic_error);
}
