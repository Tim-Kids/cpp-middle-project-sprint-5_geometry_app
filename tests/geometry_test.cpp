#include <gtest/gtest.h>

#include "geometry.hpp"

using namespace geometry;

TEST(Point2D, BasicArithmetic) {
    Point2D a{3, 4};
    Point2D b{1, 2};
    EXPECT_EQ(a + b, (Point2D{4, 6}));
    EXPECT_EQ(a - b, (Point2D{2, 2}));
    EXPECT_EQ(a * 2, (Point2D{6, 8}));
    EXPECT_EQ(a / 2, (Point2D{1.5, 2}));
}

TEST(Point2D, DotAndCross) {
    Point2D a{1, 0};
    Point2D b{0, 1};
    EXPECT_DOUBLE_EQ(a.Dot(b), 0.0);
    EXPECT_DOUBLE_EQ(a.Cross(b), 1.0);
}

TEST(Point2D, LengthAndNormalize) {
    Point2D a{3, 4};
    EXPECT_NEAR(a.Length(), 5.0, 1e-9);
    auto n = a.Normalize();
    EXPECT_NEAR(n.x, 0.6, 1e-9);
    EXPECT_NEAR(n.y, 0.8, 1e-9);
}

TEST(BoundingBox, BasicOps) {
    BoundingBox box{0, 0, 4, 3};
    EXPECT_DOUBLE_EQ(box.Width(), 4.0);
    EXPECT_DOUBLE_EQ(box.Height(), 3.0);
    EXPECT_EQ(box.Center(), (Point2D{2, 1.5}));

    BoundingBox other{3, 2, 5, 5};
    EXPECT_TRUE(box.Overlaps(other));
    BoundingBox far{10, 10, 20, 20};
    EXPECT_FALSE(box.Overlaps(far));
}

TEST(Line, Geometry) {
    Line l{{0,0},{3,4}};
    EXPECT_NEAR(l.Length(), 5.0, 1e-9);
    EXPECT_EQ(l.Center(), (Point2D{1.5,2.0}));
    auto dir = l.Direction();
    EXPECT_NEAR(dir.x, 0.6, 1e-9);
    EXPECT_NEAR(dir.y, 0.8, 1e-9);
    auto bb = l.BoundBox();
    EXPECT_DOUBLE_EQ(bb.Width(), 3.0);
    EXPECT_DOUBLE_EQ(bb.Height(), 4.0);
}

TEST(Triangle, AreaAndCenter) {
    Triangle t{{0,0},{4,0},{0,3}};
    EXPECT_NEAR(t.Area(), 6.0, 1e-9);
    EXPECT_EQ(t.Center(), (Point2D{(0+4+0)/3.0,(0+0+3)/3.0}));
    EXPECT_NEAR(t.Height(), 3.0, 1e-9);
}

TEST(Rectangle, BasicGeometry) {
    Rectangle r{{0,0}, 4, 2};
    EXPECT_NEAR(r.Area(), 8.0, 1e-9);
    EXPECT_EQ(r.Center(), (Point2D{2,1}));
    EXPECT_EQ(r.BoundBox(), (BoundingBox{0,0,4,2}));
    EXPECT_NEAR(r.Height(), 2.0, 1e-9);
}

TEST(RegularPolygon, VerticesAndBoundBox) {
    RegularPolygon p{{0,0}, 1.0, 4}; // square-like
    auto verts = p.Vertices();
    EXPECT_EQ(verts.size(), 4);
    auto bb = p.BoundBox();
    EXPECT_NEAR(bb.Width(), 2.0, 1e-9);
    EXPECT_NEAR(bb.Height(), 2.0, 1e-9);
}

TEST(Circle, BoundBoxAndVertices) {
    Circle c{{0,0}, 5.0};
    auto bb = c.BoundBox();
    EXPECT_EQ(bb, (BoundingBox{-5,-5,5,5}));
    auto verts = c.Vertices(20);
    EXPECT_EQ(verts.size(), 20);
    EXPECT_NEAR(c.Height(), 5.0, 1e-9);
}

TEST(Polygon, CenterAndBoundBox) {
    std::vector<Point2D> pts{{0,0},{4,0},{4,3},{0,3}};
    Polygon poly{pts};
    auto bb = poly.BoundBox();
    EXPECT_EQ(bb, (BoundingBox{0,0,4,3}));
    EXPECT_NEAR(poly.Height(), 3.0, 1e-9);
    auto c = poly.Center();
    EXPECT_NEAR(c.x, 2.0, 1e-9);
    EXPECT_NEAR(c.y, 1.5, 1e-9);
}
