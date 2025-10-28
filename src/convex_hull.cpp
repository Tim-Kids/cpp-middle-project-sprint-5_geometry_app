#include "convex_hull.hpp"

#include <algorithm>
#include <ranges>
#include <expected>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2) {
    auto new_p1 = p1 - middle;
    auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

GeometryResult<std::vector<Point2D>> GrahamScan(std::span<const Point2D> points) noexcept {
    if(points.size() < 3) {
        return std::unexpected(GeometryError::InsufficientPoints);
    }

    std::vector<Point2D> pts(points.begin(), points.end());

    // 1. Find the lowest, then leftmost point (pivot).
    auto it0 = std::ranges::min_element(pts, {}, [](const Point2D& p) {
        return std::pair{p.y, p.x};
    });
    std::swap(*it0, pts.front());
    Point2D p0 = pts.front();

    // 2. Sort by polar angle with p0.
    std::ranges::sort(pts.begin() + 1, pts.end(),
                      [p0](const Point2D& a, const Point2D& b) {
                          double cross = (a - p0).Cross(b - p0);
                          if(std::abs(cross) < 1e-12) {     // Collinear.
                              return p0.DistanceTo(a) < p0.DistanceTo(b);
                          }
                          return cross > 0;
                      });

    // 3. Build hull stack.
    StackForGrahamScan st;
    st.Push(pts[0]);
    st.Push(pts[1]);
    st.Push(pts[2]);

    for(size_t i = 3; i < pts.size(); ++i) {
        while(st.Size() >= 2 && CrossProduct(st.NextToTop(), st.Top(), pts[i]) <= 0) {
            st.Pop();
        }
        st.Push(pts[i]);
    }

    auto hull = std::move(st).Extract();

    // Rare case: all points almost collinear => minimal hull.
    if(hull.size() < 3) {
        // Fallback: take 3 extreme points to form a thin triangle
        if(points.size() >= 3) {
            std::vector<Point2D> fallback = {
                *std::ranges::min_element(points, {}, &Point2D::x),
                *std::ranges::max_element(points, {}, &Point2D::x),
                *std::ranges::max_element(points, {}, &Point2D::y)
            };
            return fallback;
        }
        return std::unexpected(GeometryError::DegenrateCase);
    }

    return hull;

}

} // namespace geometry::convex_hull
