#include <iostream>
#include "quick_hull.h"
#include "graham_hull.h"
#include "quick_hull_3d.h"
#include "draw3d.h"


// Demo usage
int main() {
    //-----------------quick hulll 2D------------

    // std::vector<Point> points = {
    //     {0,0}, {1,1}, {2,2}, {0,3}, {3,0}, {3,3}, {1,2}
    // };

    std::vector<Point> points = { {0,0}, {2,0}, {1,2} };
    // { {0,0}, {4,0}, {4,1}, {2,1}, {2,2}, {4,2}, {4,4}, {0,4}, {3,0.5}, {1,3.5}, {2,2.5}, {4,4}, {0,0} };

    auto qHull = quickHull(points);

    auto gHull = grahamHull(points);

    std::cout << "Convex Hull 2d points (QuickHull 2d):\n";
    for (auto& p : qHull)
        std::cout << "(" << p.x << "," << p.y << ") ";
    std::cout << std::endl;


    for (auto& p : gHull)
        std::cout << "(" << p.x << "," << p.y << ") ";
    std::cout << std::endl;

    //-----------------quick hulll 3D------------
    std::cout << "\nConvex Hull 3d points (QuickHull 3d):\n";
    
    using namespace qh3d;
    std::vector<Vec3> pts = {
        {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1},
        {1,1,1}, {1,-1,0}, {-1,0,1}, {0.2,0.2,0.2}
    };

    auto faces = convex_hull_3d(pts, 1e-9);

    std::cout << "Hull has " << faces.size() << " triangular faces:\n";
    for (auto& f : faces) {
        std::cout << "{" << f[0] << "," << f[1] << "," << f[2] << "}, ";
    }

    std::cout << std::endl;

    //---------draw3d--------
    renderer3d::Convex3dDraw renderer(pts);
    if (!renderer.init()) return -1;

    renderer.renderLoop();
}
