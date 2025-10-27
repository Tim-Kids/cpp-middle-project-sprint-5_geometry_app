#include <gtest/gtest.h>
#include "shape_utils.hpp"

using namespace geometry;
using namespace geometry::utils;
using geometry::queries::BoundingBoxesOverlap;

// ------------------------------------------------------
//  Helper: quick builder for common shapes.
// ------------------------------------------------------
static Rectangle makeRect(double x, double y, double w, double h) {
    return Rectangle{Point2D{x, y}, w, h};
}

static Circle makeCircle(double x, double y, double r) {
    return Circle{Point2D{x, y}, r};
}

// ------------------------------------------------------
//  Tests for FindAllCollisions().
// ------------------------------------------------------

TEST(FindAllCollisions, TwoRectanglesOverlap) {
    std::vector<Shape> shapes;
    shapes.emplace_back(makeRect(0, 0, 4, 4));  // covers (0–4,0–4).
    shapes.emplace_back(makeRect(2, 2, 4, 4));  // overlaps.
    shapes.emplace_back(makeRect(10, 10, 2, 2));// separate.

    auto collisions = FindAllCollisions(shapes);

    // Expect only first two collide.
    ASSERT_EQ(collisions.size(), 1u);

    const auto& [a,b] = collisions.front();
    EXPECT_TRUE(BoundingBoxesOverlap(a,b));
}

TEST(FindAllCollisions, CirclesTouchAndSeparated) {
    std::vector<Shape> shapes;
    shapes.emplace_back(makeCircle(0, 0, 2.0));
    shapes.emplace_back(makeCircle(3.9, 0, 2.0));  // slightly touching (AABB overlap).
    shapes.emplace_back(makeCircle(10, 0, 2.0));   // far away.

    auto collisions = FindAllCollisions(shapes);

    // Only first two overlap in AABB.
    ASSERT_EQ(collisions.size(), 1u);
    EXPECT_TRUE(BoundingBoxesOverlap(collisions[0].first, collisions[0].second));
}

TEST(FindAllCollisions, MixedShapesNoOverlap) {
    std::vector<Shape> shapes;
    shapes.emplace_back(makeRect(0,0,2,2));
    shapes.emplace_back(makeCircle(10,10,1));
    shapes.emplace_back(makeRect(-10,-10,1,1));

    auto collisions = FindAllCollisions(shapes);
    EXPECT_TRUE(collisions.empty());
}

TEST(FindAllCollisions, DetectMultiplePairs) {
    std::vector<Shape> shapes;
    shapes.emplace_back(makeRect(0,0,5,5));   // R0.
    shapes.emplace_back(makeRect(2,2,3,3));   // R1 inside R0.
    shapes.emplace_back(makeCircle(4,4,2));   // overlaps both.
    shapes.emplace_back(makeRect(20,20,5,5)); // isolated.

    auto collisions = FindAllCollisions(shapes);

    // Expected pairs: (0,1), (0,2), (1,2).
    EXPECT_EQ(collisions.size(), 3u);

    for (auto& [a,b] : collisions)
        EXPECT_TRUE(BoundingBoxesOverlap(a,b));
}
