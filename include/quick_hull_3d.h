#ifndef QUICK_HULL_3D_H
#define QUICK_HULL_3D_H

#pragma once
#include <vector>
#include <array>
#include <cmath> 

namespace qh3d {

// -------------------- Geometry types --------------------

struct Vec3 {
    double x{}, y{}, z{};
    Vec3() = default;
    Vec3(double X,double Y,double Z):x(X),y(Y),z(Z){}
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s) const { return {x*s, y*s, z*s}; }
};

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}
inline double dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline double norm(const Vec3& v) { return std::sqrt(dot(v,v)); }

// Directed plane: n·X + d = 0 (n points outward)
struct Plane {
    Vec3 n{};
    double d{};
    inline double signedDistance(const Vec3& p) const { return dot(n,p) + d; }
};

// A triangular face on the hull
struct Face {
    // indices into points array, oriented CCW when viewed from outside
    std::array<int,3> v{};
    Plane plane{};
    // points outside this face (candidates) stored as indices
    std::vector<int> outside;
    bool alive{true};
};

// Undirected edge key for hashing (min, max)
struct UEdge {
    int a, b;
    UEdge(int i, int j) { if (i<j){a=i;b=j;} else {a=j;b=i;} }
    bool operator==(const UEdge& o) const { return a==o.a && b==o.b; }
};
struct UEdgeHash {
    size_t operator()(const UEdge& e) const {
        return (size_t)e.a * 11400714819323198485ull ^ (size_t)e.b;
    }
};

// -------------------- QuickHull 3D --------------------

struct QuickHull3D {
    const std::vector<Vec3>& pts;
    double eps; // tolerance
    std::vector<Face> faces;

    QuickHull3D(const std::vector<Vec3>& points, double epsilon=1e-9);

    // public API: compute convex hull faces (as triplets of indices)
    std::vector<std::array<int,3>> compute();

private:
    // choose initial tetrahedron: four non-coplanar extreme points
    std::array<int,4> initialTetrahedron();

    Plane planeFrom(const Vec3& a, const Vec3& b, const Vec3& c) const;

    // Ensure face orientation is outward w.r.t. given reference point (centroid of tetra)
    void orientFaceOutward(Face& f, const Vec3& ref);

    void initTetraFaces(const std::array<int,4>& T);

    void assignOutsidePoints();

    // pick a face that currently has outside points
    int pickFaceWithOutside() const;

    // find farthest point from a face among its outside set
    int farthestPointFromFace(const Face& f) const;

    // mark all faces visible from point p
    void collectVisibleFaces(int p, std::vector<int>& visible) const;

    // Find horizon as list of directed edges (u->v) bordering a visible and a non-visible face
    // We collect undirected edge counts among visible faces; edges with count==1 lie on boundary.
    // Then we orient each boundary edge so that the winding with the new apex will be CCW outward.
    std::vector<std::pair<int,int>> computeHorizon(const std::vector<int>& visible);

    // Reassign points from a set of removed faces to the new faces' outside sets
    void reassignOutsidePoints(
        const std::vector<int>& removedFaces,
        const std::vector<int>& newFaceIdx);

    void expand();
};

inline std::vector<std::array<int,3>> 
convex_hull_3d(const std::vector<Vec3>& points, double eps=1e-9)
{
    QuickHull3D qh(points, eps);
    return qh.compute();
}

} // namespace qh3d




#endif