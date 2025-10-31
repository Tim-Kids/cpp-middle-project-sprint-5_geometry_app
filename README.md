# Geometry Library

[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-26-blue.svg)](https://en.cppreference.com/w/)
[![Tests](https://img.shields.io/badge/tests-passing-success.svg)]()
[![Matplot++](https://img.shields.io/badge/visualization-Matplot%2B%2B-purple.svg)](https://github.com/alandefreitas/matplotplusplus)

> Modern C++26 geometry library providing a set of 2D primitives, analytical tools, and geometric algorithms built with zero-cost abstractions and expressive functional design.

---

## 🧭 Overview

The **Geometry Library** is a modular C++ toolkit for two-dimensional computational geometry.  
It demonstrates how to model, analyze, and visualize geometric primitives using **modern language features** — `std::variant`, `std::expected`, `std::optional`, and `std::ranges` — while keeping a clean, production-grade design.

---

## ✨ Features

- **Shape primitives**
  - `Point2D`, `Line`, `Triangle`, `Rectangle`, `RegularPolygon`, `Circle`, `Polygon`
- **Algorithms**
  - Convex Hull via *Graham Scan*
  - Bounding Box Collision Detection
  - Delaunay Triangulation via *Bowyer–Watson*
- **Query and visitor utilities**
  - Intersection and distance visitors
  - Bounding box, height, and overlap queries
- **Functional and modern C++**
  - `std::variant`-based polymorphism (no virtual tables)
  - `std::expected` for error handling
  - Declarative transformations via `std::ranges`
- **Visualization**
  - Interactive plotting through [Matplot++](https://github.com/alandefreitas/matplotplusplus)
- **Testing**
  - Comprehensive GTest suite covering all modules

---

## ⚙️ Build Instructions

### Requirements

| Component | Minimum Version |
|------------|----------------|
| **CMake** | 3.30 |
| **Compiler** | GCC 15 / Clang 18 / MSVC 2025 (C++26) |
| **GTest** | Auto-fetched |
| **Matplot++** | v1.2.2 (via CPM.cmake) |

### Build and Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/GeometryApp
```

### Run Tests

```bash
cd build
ctest --output-on-failure
```

## 🧪 Example Output

```
Generated 15 random shapes

=== Shape Analysis ===
Обнаружены коллизии (пересечения Bounding Box):
  Rectangle(...) пересекается с Circle(...)
Самая высокая фигура: #7 (высота = 61.45)
Поддерживаемые расстояния между фигурами:
  Расстояние между фигурами #0 и #5 = 12.313

Convex hull has 8 vertices
Delaunay produced 6 triangles
```

---

## 🧰 Technologies and Standards

| Category | Tools / Concepts |
|-----------|------------------|
| **Language** | C++26, Ranges, Concepts, `std::expected`, `std::variant`, `std::optional` |
| **Build System** | CMake 3.30 |
| **Testing** | GoogleTest |
| **Visualization** | Matplot++ |
| **Design** | Type-safe polymorphism, RAII, monadic composition |
| **Attributes** | `[[nodiscard]]`, `noexcept`, `constexpr`, `final` |

---

## 🧩 Key Components

- **Type-safe polymorphism** — using `std::variant` and visitors  
- **Functional composition** — through `GeometryResult<T>` monads  
- **Composable visitors** — for intersections, distances, and bounding boxes  
- **Custom `std::formatter` support** — for `Shape` and `std::vector<Point2D>`  
- **Analytical layer** — via `analysis_utils.hpp` (collision search, height ranking, metrics)

---

## 🚀 Development Notes

- Install **gnuplot** for Matplot++ visualization:
  ```bash
  sudo apt install gnuplot
  ```
- On CI or headless systems, comment out `visualization::Draw()` calls in `main.cpp`.

---

## 🧾 License

This project is licensed under the **MIT License**.  
See [LICENSE](LICENSE) for full text.

