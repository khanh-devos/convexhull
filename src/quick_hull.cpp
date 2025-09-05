#include <vector>
#include <cmath>
#include <algorithm>
#include "point.h"



// Recursive helper: find hull points on one side and between 2 points A and B.
void quickHullRec(const std::vector<Point>& pts, const Point& A, const Point& B,
                  std::vector<Point>& hull) {
    int idx = -1;
    double maxDist = 0;

    for (size_t i = 0; i < pts.size(); i++) {
        double d = distance(A, B, pts[i]);
        if (cross(A, B, pts[i]) > 0 && d > maxDist) {
            idx = i;
            maxDist = d;
        }
    }

    if (idx == -1) {
        // No point is left → A and B form part of hull
        hull.push_back(B);
        return;
    }

    // Recurse: farthest point found
    quickHullRec(pts, A, pts[idx], hull);
    quickHullRec(pts, pts[idx], B, hull);
}

// QuickHull main: define the closest point A and the furthest point B based on X asis.
// run recursively at 2 parts: lower (A->B) and upper (B->A)
std::vector<Point> quickHull(std::vector<Point> pts) {
    if (pts.size() < 3) return pts;

    // Find leftmost and rightmost points
    auto minmaxX = std::minmax_element(pts.begin(), pts.end(),
        [](const Point& a, const Point& b) { return a.x < b.x; });
    Point A = *minmaxX.first, B = *minmaxX.second;

    std::vector<Point> hull;
    hull.push_back(A);

    quickHullRec(pts, A, B, hull); // Upper side
    quickHullRec(pts, B, A, hull); // Lower side

    return hull;
}