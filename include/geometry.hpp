#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <expected>
#include <format>
#include <numbers>
#include <optional>
#include <print>
#include <ranges>
#include <variant>
#include <vector>

namespace geometry {

/*
 * Добавьте к методам класса Point2D и Lines2DDyn все необходимые аттрибуты и спецификаторы
 * Важно: Возвращаемый тип и принимаемые аргументы менять не нужно
 */
struct Point2D {
    double x, y;

    constexpr Point2D() noexcept :
        x(0),
        y(0) {}

    constexpr Point2D(double x_, double y_) noexcept :
        x(x_),
        y(y_) {}

    // Comparison
    [[nodiscard]] constexpr bool operator<(const Point2D& other) const noexcept {
        return x < other.x && y < other.y; // как в шаблоне
    }

    [[nodiscard]] constexpr bool operator==(const Point2D& other) const noexcept {
        return x == other.x && y == other.y;
    }

    // Binary math operators
    [[nodiscard]] constexpr Point2D operator+(const Point2D& other) const noexcept {
        return {x + other.x, y + other.y};
    }

    [[nodiscard]] constexpr Point2D operator-(const Point2D& other) const noexcept {
        return {x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr Point2D operator*(double value) const noexcept {
        return {x * value, y * value};
    }

    [[nodiscard]] constexpr Point2D operator/(double value) const noexcept {
        return {x / value, y / value};
    }

    // Binary geometry operations
    [[nodiscard]] constexpr double Dot(const Point2D& other) const noexcept {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] constexpr double Cross(const Point2D& other) const noexcept {
        return x * other.y - y * other.x;
    }

    [[nodiscard]] double Length() const noexcept {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] double DistanceTo(const Point2D& other) const noexcept {
        return (*this - other).Length();
    }

    [[nodiscard]] Point2D Normalize() const noexcept {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template<size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    void Reserve(size_t n) {
        x.reserve(n);
        y.reserve(n);
    }

    void PushBack(Point2D p) {
        x.push_back(p.x);
        y.push_back(p.y);
    }

    void PushBack(double px, double py) {
        x.push_back(px);
        y.push_back(py);
    }

    [[nonodiscard]] Point2D Front() {
        return {x.front(), y.front()};
    }
};

struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    /* ваш код здесь */

    [[nodiscard]] constexpr bool Overlaps(const BoundingBox& o) const noexcept {
        return !(max_x < o.min_x || o.max_x < min_x || max_y < o.min_y || o.max_y < min_y);
    }

    [[nodiscard]] constexpr double Width() const noexcept {
        return max_x - min_x;
    }

    [[nodiscard]] constexpr double Height() const noexcept {
        return max_y - min_y;
    }

    [[nodiscard]] constexpr Point2D Center() const noexcept {
        return {(min_x + max_x) * 0.5, (min_y + max_y) * 0.5};
    }

    bool operator==(const BoundingBox& other) const noexcept {
        return max_x == other.max_x && max_y == other.max_y;
    }

};

struct Line {
    Point2D start, end;

    [[nodiscard]] double Length() const noexcept {
        return start.DistanceTo(end);
    }

    [[nodiscard]] Point2D Direction() const noexcept {
        return (end - start).Normalize();
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        const double minx = std::min(start.x, end.x);
        const double maxx = std::max(start.x, end.x);
        const double miny = std::min(start.y, end.y);
        const double maxy = std::max(start.y, end.y);
        return {minx, miny, maxx, maxy};
    }

    [[nodiscard]] double Height() const noexcept {
        return std::max(start.y, end.y);
    }

    [[nodiscard]] Point2D Center() const noexcept {
        return (start + end) / 2.0;
    }

    [[nodiscard]] std::array<Point2D, 2> Vertices() const noexcept {
        return {Point2D{start.x, start.y}, {end.x, end.y}};
    }

    [[nodiscard]] Lines2D<2> Lines() const noexcept {
        return {{start.x, end.x}, {start.y, end.y}};
    }
};

struct Triangle {
    Point2D a, b, c;

    [[nodiscard]] double Area() const noexcept {
        // 0.5 * |(b-a) x (c-a)|
        return std::abs((b - a).Cross(c - a)) * 0.5;
    }

    [[nodiscard]] double Height() const noexcept {
        return std::max({a.y, b.y, c.y});
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        const double minx = std::min({a.x, b.x, c.x});
        const double maxx = std::max({a.x, b.x, c.x});
        const double miny = std::min({a.y, b.y, c.y});
        const double maxy = std::max({a.y, b.y, c.y});
        return {minx, miny, maxx, maxy};
    }

    //
    // Обратите внимание! В методе Lines(), в отличие от Vertices(), координаты точек замыкаются на начало:
    // a.x, b.x, c.x а затем идёт снова первая вершина a.x
    //
    // Это необходимо для правильного рисования фигур через gnuplot, который формирует линии, используя пары точек.
    // В случае с Triangle будут составлены такие пары точек:
    //      - { a, b }
    //      - { b, c }
    //      - { c, a }
    //

    /* ваш код здесь */

    [[nodiscard]] Point2D Center() const noexcept {
        return (a + b + c) / 3.0;
    }

    [[nodiscard]] std::array<Point2D, 3> Vertices() const noexcept {
        return {a, b, c};
    }

    [[nodiscard]] Lines2D<4> Lines() const noexcept {
        return {{a.x, b.x, c.x, a.x}, {a.y, b.y, c.y, a.y}};
    }
};

struct Rectangle {
    Point2D bottom_left{};
    double width{};
    double height{};

    [[nodiscard]] double Area() const noexcept {
        return width * height;
    }

    [[nodiscard]] double Height() const noexcept {
        return bottom_left.y + height;
    }

    [[nodiscard]] Point2D Center() const noexcept {
        return {bottom_left.x + width * 0.5, bottom_left.y + height * 0.5};
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {bottom_left.x, bottom_left.y, bottom_left.x + width, bottom_left.y + height};
    }

    [[nodiscard]] std::array<Point2D, 4> Vertices() const noexcept {
        const Point2D bl = bottom_left;
        const Point2D br{bottom_left.x + width, bottom_left.y};
        const Point2D tr{bottom_left.x + width, bottom_left.y + height};
        const Point2D tl{bottom_left.x, bottom_left.y + height};
        return {bl, br, tr, tl};
    }

    [[nodiscard]] Lines2D<5> Lines() const noexcept {
        const auto v = Vertices();
        return {{v[0].x, v[1].x, v[2].x, v[3].x, v[0].x},
                {v[0].y, v[1].y, v[2].y, v[3].y, v[0].y}};
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius_, int sides_) noexcept :
        center_p(center),
        radius(radius_),
        sides(sides_) {
    }

    [[nodiscard]] double Height() const noexcept {
        return center_p.y + radius;
    }

    [[nodiscard]] Point2D Center() const noexcept {
        return center_p;
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }

    [[nodiscard]] std::vector<Point2D> Vertices() const {
        std::vector<Point2D> points;
        points.reserve(static_cast<size_t>(sides));
        for(int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * static_cast<double>(i) / static_cast<double>(sides);
            points.emplace_back(center_p.x + radius * std::cos(angle),
                                center_p.y + radius * std::sin(angle));
        }
        return points;
    }

    [[nodiscard]] Lines2DDyn Lines() const {
        Lines2DDyn out;
        if(sides <= 0) {
            return out;
        }
        const auto verts = Vertices();
        out.Reserve(static_cast<size_t>(sides) + 1);
        for(const auto& p: verts) {
            out.PushBack(p);
        }
        out.PushBack(verts.front()); // Замыкание координаты точек на начало для корректной отрисовки.
        return out;
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius_) noexcept :
        center_p(center),
        radius(radius_) {
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }

    [[nodiscard]] double Height() const noexcept {
        return center_p.y + radius;
    }

    [[nodiscard]] Point2D Center() const noexcept {
        return center_p;
    }

    //
    // Должны быть сделана по аналогии с RegularPolygon::Vertices
    //

    [[nodiscard]] std::vector<Point2D> Vertices(size_t N = 30) const {
        std::vector<Point2D> pts;
        if(N == 0) {
            return pts;
        }
        pts.reserve(N);
        for(size_t i = 0; i < N; ++i) {
            const double angle = 2 * std::numbers::pi * static_cast<double>(i) / static_cast<double>(N);
            pts.emplace_back(center_p.x + radius * std::cos(angle),
                             center_p.y + radius * std::sin(angle));
        }
        return pts;
    }

    [[nodiscard]] Lines2DDyn Lines(size_t N = 100) const {
        Lines2DDyn out;
        if(N == 0) {
            return out;
        }
        const auto verts = Vertices(N);
        out.Reserve(N + 1);
        for(const auto& p: verts) {
            out.PushBack(p);
        }
        out.PushBack(verts.front()); // Замыкание координаты точек на начало для корректной отрисовки.
        return out;
    }
};

class Polygon {
    public:
    Polygon() = default;

    explicit Polygon(std::vector<Point2D> pts) :
        points_(std::move(pts)) {
        bounding_box_ = computeBoundingBox();
    }

    [[nodiscard]] double Height() const noexcept {
        if(points_.empty())
            return 0.0;
        double maxy = points_.front().y;
        for(const auto& p: points_) {
            maxy = std::max(maxy, p.y);
        }
        return maxy;
    }

    [[nodiscard]] Point2D Center() const noexcept {
        if(points_.empty())
            return {0, 0};
        // Центроид полигона (приблизительно для невырожденных многоугольников)
        double A       = 0.0;
        double Cx      = 0.0, Cy = 0.0;
        const size_t n = points_.size();
        for(size_t i = 0; i < n; ++i) {
            const auto& p      = points_[i];
            const auto& q      = points_[(i + 1) % n];
            const double cross = p.x * q.y - q.x * p.y;
            A += cross;
            Cx += (p.x + q.x) * cross;
            Cy += (p.y + q.y) * cross;
        }
        A *= 0.5;
        if(std::abs(A) < 1e-12) {
            // fallback: среднее вершин
            Point2D acc{0, 0};
            for(auto& p: points_) {
                acc = acc + p;
            }
            return acc / static_cast<double>(n);
        }
        Cx /= (6.0 * A);
        Cy /= (6.0 * A);
        return {Cx, Cy};
    }

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return bounding_box_;
    }

    //
    // Должны быть сделана по аналогии с RegularPolygon::Vertices
    //

    [[nodiscard]] std::vector<Point2D> Vertices(size_t N = 30) const {
        return points_;
    }

    [[nodiscard]] Lines2DDyn Lines(size_t N = 100) const {
        Lines2DDyn out;
        if(points_.empty()) {
            return out;
        }
        out.Reserve(points_.size() + 1);
        for(const auto& p: points_) {
            out.PushBack(p);
        }
        out.PushBack(points_.front()); // Замыкание координаты точек на начало для корректной отрисовки.
        return out;
    }

    private:
    [[nodiscard]] BoundingBox computeBoundingBox() const noexcept {
        if(points_.empty()) {
            return {0, 0, 0, 0};
        }
        double minx = points_.front().x, maxx = points_.front().x;
        double miny = points_.front().y, maxy = points_.front().y;
        for(const auto& p: points_) {
            minx = std::min(minx, p.x);
            miny = std::min(miny, p.y);
            maxx = std::max(maxx, p.x);
            maxy = std::max(maxy, p.y);
        }
        return {minx, miny, maxx, maxy};
    }

    private:
    std::vector<Point2D> points_;
    BoundingBox bounding_box_{0, 0, 0, 0};
};

using Shape = std::variant<Line, Triangle, Rectangle, RegularPolygon, Circle, Polygon>;

enum class GeometryError { Unsupported, NoIntersection, InvalidInput, DegenrateCase, InsufficientPoints };

template<typename T>
using GeometryResult = std::expected<T, GeometryError>;

} // namespace geometry

template<>
struct std::formatter<geometry::Point2D> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Point2D& p, FormatContext& ctx) const {
        return format_to(ctx.out(), "({:.2f}, {:.2f})", p.x, p.y);
    }
};

template<>
struct std::formatter<std::vector<geometry::Point2D>> {
    bool use_new_line = false;

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it == ':') {
            ++it;
            if (std::string_view(it, end - it).starts_with("new_line")) {
                use_new_line = true;
                it += std::string_view("new_line").size();
            }
        }
        if(it != end && *it != '}') {
            throw std::format_error("invalid format for vector<Point2D>");
        }
        return it;
    }

    template<typename FormatContext>
    auto format(const std::vector<geometry::Point2D>& points, FormatContext& ctx) const {
        auto out = ctx.out();
        if (points.empty())
            return std::format_to(out, "[]");

        if (use_new_line) {
            out = std::format_to(out, "[\n");
            for (const auto& p : points) {
                out = std::format_to(out, "\t{}\n", p);
            }
            return std::format_to(out, "]");
        }
        else {
            out = std::format_to(out, "[ ");
            for(size_t i = 0; i < points.size(); ++i) {
                out = std::format_to(out, "{}", points[i]);
                if(i + 1 < points.size()) {
                    out = std::format_to(out, ", ");
                }
            }
            return std::format_to(out, " ]");
        }
    }
};

template<>
struct std::formatter<geometry::Line> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Line& l, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Line({}, {})", l.start, l.end);
    }
};

template<>
struct std::formatter<geometry::Circle> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Circle& c, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Circle(center={}, r={:.2f})", c.center_p, c.radius);
    }
};

template<>
struct std::formatter<geometry::Rectangle> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Rectangle& r, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Rectangle(bottom_left={}, w={:.2f}, h={:.2f})", r.bottom_left, r.width,
                              r.height);
    }
};

template<>
struct std::formatter<geometry::RegularPolygon> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::RegularPolygon& p, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "RegularPolygon(center={}, r={:.2f}, sides={})", p.center_p, p.radius,
                              p.sides);
    }
};

template<>
struct std::formatter<geometry::Triangle> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Triangle& t, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "Triangle({}, {}, {})", t.a, t.b, t.c);
    }
};

template<>
struct std::formatter<geometry::Polygon> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Polygon& poly, FormatContext& ctx) const {
        auto out = ctx.out();
        out      = std::format_to(out, "Polygon[{} points]: [", poly.Vertices().size());

        for(const auto& p: poly.Vertices()) {
            out = std::format_to(out, "{} ", p);
        }

        return std::format_to(out, "]");
    }
};

template<>
struct std::formatter<geometry::Shape> {
    constexpr auto parse(std::format_parse_context& ctx) const {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const geometry::Shape& shape, FormatContext& ctx) const {
        auto out = ctx.out();
        return std::visit([&](const auto& s) {
            return std::format_to(out, "{}", s);
        }, shape);
    }
};
