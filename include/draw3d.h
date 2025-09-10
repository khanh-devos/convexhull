#ifndef DRAW3D_H
#define DRAW3D_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include "quick_hull_3d.h"

namespace renderer3d {

class Convex3dDraw {
public:
    Convex3dDraw(const std::vector<qh3d::Vec3>& points);

    // Initialize OpenGL (GLFW + GLAD)
    bool init(int width = 800, int height = 600, const char* title = "3D Convex Hull");

    // Main render loop
    void renderLoop();

private:
    std::vector<qh3d::Vec3> points_;
    std::vector<std::array<int,3>> faces_;
    void drawHull();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    GLFWwindow* window_ = nullptr;
};

} // namespace renderer3d

#endif
