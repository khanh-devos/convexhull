#ifndef POINT_H
#define POINT_H

#include <vector>
#include <cmath>
#include <algorithm>

// EPS tolerance for floating-point comparisons
const double EPS = 1e-9;

struct Point {
    double x, y;
    bool operator==(const Point& other) const {
        return std::fabs(x - other.x) < EPS &&
               std::fabs(y - other.y) < EPS;
    }
};

// Compute cross product of OA × OB
inline double cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Distance from point P to line AB
inline double distance(const Point& A, const Point& B, const Point& P) {
    double area = std::fabs(cross(A, B, P));
    double base = std::hypot(B.x - A.x, B.y - A.y);
    return area / (base + EPS);     // add EPS to avoid div/0
}

// double x1 = 0.1 + 0.2;  // 0.3 expected, but it's not
// x1 == 0.3 => false, so we need define "almostEqual" func
inline bool almostEqual(double a, double b, double eps = 1e-6) {
    return std::abs(a - b) < eps;
}

#endif