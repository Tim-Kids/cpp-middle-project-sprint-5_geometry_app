#pragma once

#include "geometry.hpp"

#include <algorithm>
#include <ranges>
#include <stack>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2);

class StackForGrahamScan {
    public:
    void Push(const Point2D& p) {
        s.push_back(p);
    }

    void Pop() {
        s.pop_back();
    }

    [[nodiscard]] size_t Size() const noexcept {
        return s.size();
    }

    [[nodiscard]] Point2D Top() const noexcept {
        return s.back();
    }

    [[nodiscard]] Point2D NextToTop() const noexcept {
        return *(s.end() - 2);
    }

    [[nodiscard]] std::vector<Point2D>& Data() noexcept {
        return s;
    }

    [[nodiscard]] std::vector<Point2D>&& Extract() && noexcept {
        return std::move(s);
    }

    private:
    std::vector<Point2D> s{};
};

[[nodiscard]] GeometryResult<std::vector<Point2D>> GrahamScan(std::span<const Point2D> points) noexcept;

}  // namespace geometry::convex_hull