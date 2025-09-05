#include <iostream>
#include "quick_hull.h"
#include "graham_hull.h"


// Demo usage
int main() {
    std::vector<Point> points = {
        {0,0}, {1,1}, {2,2}, {0,3}, {3,0}, {3,3}, {1,2}
    };

    auto qHull = quickHull(points);

    auto gHull = grahamHull(points);

    std::cout << "Convex Hull points (QuickHull):\n";
    for (auto& p : qHull)
        std::cout << "(" << p.x << "," << p.y << ") ";
    std::cout << std::endl;


    for (auto& p : gHull)
        std::cout << "(" << p.x << "," << p.y << ") ";
    std::cout << std::endl;

}
