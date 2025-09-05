#ifndef DRAW_H
#define DRAW_H

#include <vector>
#include "point.h"

void draw(const std::vector<Point>& original_points, 
          const std::vector<Point>& original_hull,
          int width = 600, int height = 600);

#endif