#include <vector>
#include <array>
#include <limits>
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <stdexcept>
#include "quick_hull_3d.h"

namespace qh3d {
// -------------------- QuickHull 3D --------------------
    QuickHull3D::QuickHull3D(const std::vector<Vec3>& points, double epsilon)
        : pts(points), eps(epsilon) {}

    // public API: compute convex hull faces (as triplets of indices)
    std::vector<std::array<int,3>> QuickHull3D::compute() {
        if (pts.size() < 4) return {}; // degenerate

        // 1) Build initial tetrahedron
        std::array<int,4> base = initialTetrahedron();
        initTetraFaces(base);

        // 2) Assign all other points to a face's outside set
        assignOutsidePoints();

        // 3) Expand hull
        expand();

        // 4) Collect final faces
        std::vector<std::array<int,3>> out;
        out.reserve(faces.size());
        for (auto& f : faces) if (f.alive) out.push_back(f.v);
        return out;
    }

    // choose initial tetrahedron: four non-coplanar extreme points
    std::array<int,4> QuickHull3D::initialTetrahedron() {
        const int n = (int)pts.size();

        // pick extremes on x to get a baseline
        int i_min_x=0, i_max_x=0;
        for (int i=1;i<n;++i){
            if (pts[i].x < pts[i_min_x].x) i_min_x=i;
            if (pts[i].x > pts[i_max_x].x) i_max_x=i;
        }
        if (i_min_x==i_max_x) throw std::runtime_error("All points identical in X.");

        // find furthest point from line (i_min_x -> i_max_x)
        int i_far_line=-1;
        double max_dist = -1.0;
        Vec3 A = pts[i_min_x], B = pts[i_max_x];
        Vec3 AB = B - A;
        double ab2 = std::max(1e-30, dot(AB,AB));
        for (int i=0;i<n;++i) {
            if (i==i_min_x || i==i_max_x) continue;
            Vec3 AP = pts[i] - A;
            // distance from point to line
            Vec3 crossp = cross(AB, AP);
            double d = norm(crossp) / std::sqrt(ab2);
            if (d > max_dist) { max_dist = d; i_far_line = i; }
        }
        if (i_far_line<0 || max_dist < eps) throw std::runtime_error("Points are collinear.");

        // find a point that makes a non-degenerate tetrahedron
        int i_far_plane=-1;
        double max_abs_height = -1.0;
        Plane pl = planeFrom(pts[i_min_x], pts[i_max_x], pts[i_far_line]);
        for (int i=0;i<n;++i) {
            if (i==i_min_x || i==i_max_x || i==i_far_line) continue;
            double h = std::abs(pl.signedDistance(pts[i]));
            if (h > max_abs_height) { max_abs_height = h; i_far_plane = i; }
        }
        if (i_far_plane<0 || max_abs_height < eps) throw std::runtime_error("Points are coplanar.");

        return {i_min_x, i_max_x, i_far_line, i_far_plane};
    }

    Plane QuickHull3D::planeFrom(const Vec3& a, const Vec3& b, const Vec3& c) const {
        Vec3 n = cross(b-a, c-a);
        double len = norm(n);
        if (len < 1e-30) return {{0,0,0}, 0};
        n = n * (1.0/len);
        double d = -dot(n, a);
        return {n, d};
    }

    // Ensure face orientation is outward w.r.t. given reference point (centroid of tetra)
    void QuickHull3D::orientFaceOutward(Face& f, const Vec3& ref) {
        double s = f.plane.signedDistance(ref);
        if (s > 0) {
            // flip
            std::swap(f.v[1], f.v[2]);
            f.plane = planeFrom(pts[f.v[0]], pts[f.v[1]], pts[f.v[2]]);
        }
    }

    void QuickHull3D::initTetraFaces(const std::array<int,4>& T) {
        faces.clear();
        faces.reserve(64);
        Vec3 centroid = (pts[T[0]] + pts[T[1]] + pts[T[2]] + pts[T[3]]) * 0.25;

        auto make = [&](int a,int b,int c){
            Face f;
            f.v = {a,b,c};
            f.plane = planeFrom(pts[a], pts[b], pts[c]);
            f.alive = true;
            orientFaceOutward(f, centroid);
            return f;
        };

        faces.push_back(make(T[0], T[1], T[2]));
        faces.push_back(make(T[0], T[3], T[1]));
        faces.push_back(make(T[1], T[3], T[2]));
        faces.push_back(make(T[2], T[3], T[0]));
    }

    void QuickHull3D::assignOutsidePoints() {
        const int n = (int)pts.size();
        // mark tetra vertices to skip
        std::unordered_set<int> verts;
        for (auto& f : faces) for (int i=0;i<3;++i) verts.insert(f.v[i]);

        for (int i=0;i<n;++i) {
            if (verts.count(i)) continue;
            // find the face with the largest positive distance
            int best = -1;
            double best_d = eps;
            for (int fi=0; fi<(int)faces.size(); ++fi) {
                Face& f = faces[fi];
                if (!f.alive) continue;
                double d = f.plane.signedDistance(pts[i]);
                if (d > best_d) { best_d = d; best = fi; }
            }
            if (best >= 0) faces[best].outside.push_back(i);
        }
    }

    // pick a face that currently has outside points
    int QuickHull3D::pickFaceWithOutside() const {
        int best = -1;
        double best_far = -1.0;
        for (int i=0;i<(int)faces.size();++i) {
            const Face& f = faces[i];
            if (!f.alive || f.outside.empty()) continue;
            // heuristic: use farthest point distance to prioritize
            double maxd = 0;
            for (int pi : f.outside) {
                maxd = std::max(maxd, f.plane.signedDistance(pts[pi]));
            }
            if (maxd > best_far) { best_far = maxd; best = i; }
        }
        return best;
    }

    // find farthest point from a face among its outside set
    int QuickHull3D::farthestPointFromFace(const Face& f) const {
        int far = -1;
        double best = -1.0;
        for (int pi : f.outside) {
            double d = f.plane.signedDistance(pts[pi]);
            if (d > best) { best = d; far = pi; }
        }
        return far;
    }

    // mark all faces visible from point p
    void QuickHull3D::collectVisibleFaces(int p, std::vector<int>& visible) const {
        visible.clear();
        for (int i=0;i<(int)faces.size(); ++i) {
            const Face& f = faces[i];
            if (!f.alive) continue;
            if (f.plane.signedDistance(pts[p]) > eps) visible.push_back(i);
        }
    }

    // Find horizon as list of directed edges (u->v) bordering a visible and a non-visible face
    // We collect undirected edge counts among visible faces; edges with count==1 lie on boundary.
    // Then we orient each boundary edge so that the winding with the new apex will be CCW outward.
    std::vector<std::pair<int,int>>
    QuickHull3D::computeHorizon(const std::vector<int>& visible) {
        // mark visible set
        std::unordered_set<int> vis(visible.begin(), visible.end());

        // count undirected edges inside visible faces
        std::unordered_map<UEdge,int,UEdgeHash> cnt;
        auto add_edge = [&](int a,int b){
            cnt[UEdge(a,b)]++;
        };

        for (int fi : visible) {
            const auto& f = faces[fi];
            add_edge(f.v[0], f.v[1]);
            add_edge(f.v[1], f.v[2]);
            add_edge(f.v[2], f.v[0]);
        }

        // Any undirected edge appearing exactly once is part of the horizon.
        // We need to retrieve it as a directed cycle. Build adjacency to order them.
        std::unordered_map<int, std::vector<int>> adj;
        for (auto& kv : cnt) {
            if (kv.second == 1) {
                int a = kv.first.a, b = kv.first.b;
                adj[a].push_back(b);
                adj[b].push_back(a);
            }
        }

        // Start from a vertex with degree 1 (open chain) or any (cycle)
        int start = -1;
        for (auto& kv : adj) { if (kv.second.size() == 1) { start = kv.first; break; } }
        if (start == -1 && !adj.empty()) start = adj.begin()->first;

        // Build an ordered edge loop/path
        std::vector<int> ordered;
        if (start != -1) {
            int prev = -1, cur = start;
            while (true) {
                ordered.push_back(cur);
                auto& nbrs = adj[cur];
                int next = -1;
                for (int nb : nbrs) if (nb != prev) { next = nb; break; }
                if (next == -1) break;
                prev = cur; cur = next;
                if (cur == start) { ordered.push_back(cur); break; } // closed loop
            }
        }

        // Convert to directed edges along the path (or loop)
        std::vector<std::pair<int,int>> horizon;
        for (size_t i=1;i<ordered.size();++i)
            horizon.emplace_back(ordered[i-1], ordered[i]);

        return horizon;
    }

    // Reassign points from a set of removed faces to the new faces' outside sets
    void QuickHull3D::reassignOutsidePoints(
        const std::vector<int>& removedFaces,
        const std::vector<int>& newFaceIdx)
    {
        // gather all candidate points
        std::vector<int> candidates;
        {
            std::unordered_set<int> seen;
            for (int fi : removedFaces) {
                for (int p : faces[fi].outside) if (!seen.count(p)) {
                    seen.insert(p); candidates.push_back(p);
                }
                faces[fi].outside.clear();
            }
        }
        if (candidates.empty() || newFaceIdx.empty()) return;

        for (int p : candidates) {
            int best = -1;
            double bestd = eps;
            for (int fi : newFaceIdx) {
                Face& f = faces[fi];
                double d = f.plane.signedDistance(pts[p]);
                if (d > bestd) { bestd = d; best = fi; }
            }
            if (best >= 0) faces[best].outside.push_back(p);
        }
    }

    void QuickHull3D::expand() {
        while (true) {
            int fi = pickFaceWithOutside();
            if (fi < 0) break;

            Face& base = faces[fi];
            int apex = farthestPointFromFace(base);
            if (apex < 0) { base.outside.clear(); continue; }

            // 1) collect all faces visible from apex
            std::vector<int> visible;
            collectVisibleFaces(apex, visible);

            // 2) compute horizon edges
            auto horizon = computeHorizon(visible);

            // 3) deactivate visible faces
            for (int vfi : visible) faces[vfi].alive = false;

            // 4) create new faces from horizon edges to apex
            // ensure outward orientation: for edge (u->v), make face (u, v, apex).
            // We'll flip later if orientation is incorrect.
            std::vector<int> newFaces;
            newFaces.reserve(horizon.size());

            // choose a reference point likely inside hull (average of one active face)
            Vec3 ref{0,0,0};
            bool ref_found=false;
            for (auto& f : faces) if (f.alive) {
                ref = (pts[f.v[0]] + pts[f.v[1]] + pts[f.v[2]]) * (1.0/3.0);
                ref_found=true; break;
            }
            if (!ref_found) {
                // fallback: average of apex and any three points
                ref = pts[apex];
            }

            for (auto [u,v] : horizon) {
                Face nf;
                nf.v = {u, v, apex};
                nf.plane = planeFrom(pts[u], pts[v], pts[apex]);
                nf.alive = true;
                // flip if not outward relative to ref (ref should be inside -> negative side)
                if (nf.plane.signedDistance(ref) > 0) {
                    std::swap(nf.v[0], nf.v[1]);
                    nf.plane = planeFrom(pts[nf.v[0]], pts[nf.v[1]], pts[nf.v[2]]);
                }
                newFaces.push_back((int)faces.size());
                faces.push_back(std::move(nf));
            }

            // 5) reassign outside points of removed faces to new faces
            reassignOutsidePoints(visible, newFaces);
        }
    }

} // namespace qh3d
