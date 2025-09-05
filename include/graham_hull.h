#ifndef GRAHAM_HULL_H
#define GRAHAM_HULL_H

#include <vector>
#include <algorithm>
#include <stack>
#include <cmath>
#include "draw.h"
#include "point.h"


// Graham Scan Convex Hull, using cross product and its rotation feature
// feature (counter-clockwise :: positive) to collect all right furthest points
// while iterate along X axis forwards and backwards: lower part and upper part 
std::vector<Point> grahamHull(std::vector<Point> points);


#endif