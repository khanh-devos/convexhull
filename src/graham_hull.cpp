#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
#include "draw.h"
#include "point.h"


// Graham Scan Convex Hull
std::vector<Point> grahamHull(std::vector<Point> points) {
    int n = points.size();
    if (n <= 1) return points;

    // Sort points by x, then by y
    std::sort(points.begin(), points.end(), [](const Point &a, const Point &b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });


    std::vector<Point> hull(2*n);
    int k = 0;

    // Build lower hull
    for (int i = 0; i < n; ++i) {
        
        while (k >= 2 && cross(hull[k-2], hull[k-1], points[i]) <= 0) {
            k--;
        }
        hull[k++] = points[i];
    }


    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; --i) {
        while (k >= t && cross(hull[k-2], hull[k-1], points[i]) <= 0) k--;
        hull[k++] = points[i];
    }

    hull.resize(k-1);
    return hull;
}