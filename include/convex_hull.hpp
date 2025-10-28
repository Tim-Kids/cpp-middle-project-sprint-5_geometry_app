#pragma once

#include "geometry.hpp"

#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2);

class StackForGrahamScan {
    public:
    void Push(const Point2D& p) {
        points_.push_back(p);
    }

    void Pop() {
        points_.pop_back();
    }

    [[nodiscard]] size_t Size() const noexcept {
        return points_.size();
    }

    [[nodiscard]] Point2D Top() const noexcept {
        return points_.back();
    }

    [[nodiscard]] Point2D NextToTop() const noexcept {
        return *(points_.end() - 2);
    }

    [[nodiscard]] std::vector<Point2D>& Data() noexcept {
        return points_;
    }

    [[nodiscard]] std::vector<Point2D>&& Extract() && noexcept {
        return std::move(points_);
    }

    private:
    std::vector<Point2D> points_{};
};

[[nodiscard]] GeometryResult<std::vector<Point2D>> GrahamScan(std::span<const Point2D> points) noexcept;

}  // namespace geometry::convex_hull