#include <gtest/gtest.h>
#include <cmath>
#include <set>
#include "quick_hull_3d.h"

using namespace qh3d;

// Helper: compute triangle area in 3D
static double triangleArea(const Vec3& a, const Vec3& b, const Vec3& c) {
    Vec3 ab = b - a, ac = c - a;
    Vec3 crossp = cross(ab, ac);
    return 0.5 * norm(crossp);
}

// Helper: check if a point is inside or on the hull (all faces)
static bool pointInsideHull(const std::vector<Vec3>& pts, 
                            const std::vector<std::array<int,3>>& faces,
                            const Vec3& p, double eps=1e-9) {
    for (auto& f : faces) {
        Plane pl;
        pl.n = cross(pts[f[1]]-pts[f[0]], pts[f[2]]-pts[f[0]]);
        pl.d = -dot(pl.n, pts[f[0]]);
        if (pl.signedDistance(p) > eps) {
            return false; // point lies outside at least one face
        }
    }
    return true;
}

TEST(QuickHull3D, Tetrahedron) {
    std::vector<Vec3> pts = {
        {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}
    };

    auto faces = convex_hull_3d(pts);

    // A tetrahedron has 4 triangular faces
    EXPECT_EQ(faces.size(), 4);

    // Each face should have non-zero area
    for (auto& f : faces) {
        EXPECT_GT(triangleArea(pts[f[0]], pts[f[1]], pts[f[2]]), 1e-9);
    }

    // All points should be inside or on hull
    for (auto& p : pts) {
        EXPECT_TRUE(pointInsideHull(pts, faces, p));
    }
}

TEST(QuickHull3D, CubeCorners) {
    std::vector<Vec3> pts = {
        {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
        {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
    };

    auto faces = convex_hull_3d(pts);

    // A cube convex hull should have 12 triangular faces
    EXPECT_EQ(faces.size(), 12);

    // All 8 cube corners must be on or inside hull
    for (auto& p : pts) {
        EXPECT_TRUE(pointInsideHull(pts, faces, p));
    }
}

TEST(QuickHull3D, CoplanarPoints) {
    // All points on the z=0 plane
    std::vector<Vec3> pts = {
        {0,0,0}, {1,0,0}, {0,1,0}, {1,1,0}, {0.5,0.5,0}
    };

    // The implementation throws an exception for coplanar sets
    EXPECT_THROW({
        auto faces = convex_hull_3d(pts);
    }, std::runtime_error); 

}

TEST(QuickHull3D, RandomCluster) {
    std::vector<Vec3> pts = {
        {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1},
        {2,2,2}, {-1,-1,0}, {0,2,1}, {1,-1,2}
    };

    auto faces = convex_hull_3d(pts);

    // Valid hull must have at least 4 faces
    EXPECT_GE(faces.size(), 4);

    // All points inside or on hull
    for (auto& p : pts) {
        EXPECT_TRUE(pointInsideHull(pts, faces, p));
    }
}
