#include <gtest/gtest.h>

#include "triangulation.hpp"

using namespace geometry::triangulation;

TEST(DelaunayTriangulation, BasicFourPoints) {
    std::vector<geometry::Point2D> pts = {{0,0},{1,0},{0,1},{1,1}};
    auto result = DelaunayTriangulation(pts);
    ASSERT_TRUE(result.has_value());
    EXPECT_GE(result->size(), 2u);
}
