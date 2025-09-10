#include <gtest/gtest.h>
#include "quick_hull.h"

// Helper: check if a point exists in hull
bool contains(const std::vector<Point>& hull, const Point& p) {
    for (auto& q : hull)
        if (q == p) return true;
    return false;
}

TEST(QuickHullTest, TriangleCase) {
    std::vector<Point> pts = { {0,0}, {2,0}, {1,2} };

    auto hull = quickHull(pts);

    EXPECT_EQ(hull.size(), 3);
    EXPECT_TRUE(contains(hull, {0,0}));
    EXPECT_TRUE(contains(hull, {2,0}));
    EXPECT_TRUE(contains(hull, {1,2}));
}

TEST(QuickHullTest, SquareCase) {
    std::vector<Point> pts = {
        {0,0}, {0,1}, {1,0}, {1,1}, {0.5,0.5} // square + inner point
    };

    auto hull = quickHull(pts);

    EXPECT_EQ(hull.size(), 4);  // inner point excluded
    EXPECT_TRUE(contains(hull, {0,0}));
    EXPECT_TRUE(contains(hull, {0,1}));
    EXPECT_TRUE(contains(hull, {1,0}));
    EXPECT_TRUE(contains(hull, {1,1}));
}

TEST(QuickHullTest, CollinearCase) {
    std::vector<Point> pts = { {0,0}, {1,1}, {2,2}, {3,3} };

    auto hull = quickHull(pts);

    // Only extreme points remain
    EXPECT_EQ(hull.size(), 2);
    EXPECT_TRUE(contains(hull, {0,0}));
    EXPECT_TRUE(contains(hull, {3,3}));
}

TEST(QuickHullTest, ConcaveCase) {
    std::vector<Point> pts = { 
        {0,0}, {0,0}, {4,0}, {4,0}, {4,1}, {2.5,1.5}, {2,2}, {4,2}, {4.0,4.0}, {0,4} 
    };

    auto hull = quickHull(pts);

    // Only extreme points remain
    EXPECT_EQ(hull.size(), 4);
    EXPECT_TRUE(contains(hull, {0,0}));
    EXPECT_TRUE(contains(hull, {4,0}));
    EXPECT_TRUE(contains(hull, {4,4}));
    EXPECT_TRUE(contains(hull, {0,4}));

}
