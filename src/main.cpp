#include <iostream>
#include "quick_hull.h"
#include "graham_hull.h"


// Demo usage
int main() {
    // std::vector<Point> points = {
    //     {0,0}, {1,1}, {2,2}, {0,3}, {3,0}, {3,3}, {1,2}
    // };

    std::vector<Point> points = { {0,0}, {2,0}, {1,2} };
    // { {0,0}, {4,0}, {4,1}, {2,1}, {2,2}, {4,2}, {4,4}, {0,4}, {3,0.5}, {1,3.5}, {2,2.5}, {4,4}, {0,0} };

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
