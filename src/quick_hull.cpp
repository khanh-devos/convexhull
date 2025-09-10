#include <vector>
#include <cmath>
#include <algorithm>
#include "point.h"

// Deduplicate hull points
void deduplicateHull(std::vector<Point>& hull) {
    std::sort(hull.begin(), hull.end(),
              [](const Point& p1, const Point& p2) {
                  return (p1.x < p2.x) || 
                         (std::fabs(p1.x - p2.x) < 1e-9 && p1.y < p2.y);
              });
              
    hull.erase(std::unique(hull.begin(), hull.end()), hull.end());
}

// ===== Sort Counter-Clockwise =====
void sortCounterClockwise(std::vector<Point>& hull) {
    if (hull.empty()) return;

    Point centroid{0,0};
    for (auto& p : hull) {
        centroid.x += p.x;
        centroid.y += p.y;
    }
    centroid.x /= hull.size();
    centroid.y /= hull.size();

    std::sort(hull.begin(), hull.end(),
              [&](const Point& p1, const Point& p2) {
                  return std::atan2(p1.y - centroid.y, p1.x - centroid.x) <
                         std::atan2(p2.y - centroid.y, p2.x - centroid.x);
              });
}

// Recursive helper: find hull points on one side and between 2 points A and B.
void quickHullRec(const std::vector<Point>& pts, const Point& A, const Point& B,
                  std::vector<Point>& hull) {
    int idx = -1;
    double maxDist = EPS;

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
    auto [minIt, maxIt] = std::minmax_element(pts.begin(), pts.end(),
        [](const Point& a, const Point& b) { return a.x < b.x; });
    Point A = *minIt, B = *maxIt;

    std::vector<Point> hull;
    hull.push_back(A);

    quickHullRec(pts, A, B, hull); // Upper side
    quickHullRec(pts, B, A, hull); // Lower side

    //remove the duplicates of last item A because both Upper & Lower sides have it.
    deduplicateHull(hull);
    sortCounterClockwise(hull);

    return hull;
}