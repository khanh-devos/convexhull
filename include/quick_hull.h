#ifndef QUICK_HULL_H
#define QUICK_HULL_H

#include <vector>
#include <cmath>
#include <algorithm>
#include "point.h"



// Recursive helper: find hull points on one side and between 2 points A and B.
void quickHullRec(const std::vector<Point>& pts, const Point& A, const Point& B,
                  std::vector<Point>& hull);

// QuickHull main: define the closest point A and the furthest point B based on X asis.
// run recursively at 2 parts: lower (A->B) and upper (B->A)
std::vector<Point> quickHull(std::vector<Point> pts);

#endif