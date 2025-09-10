#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>            // for tan, M_PI fallback
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "draw3d.h"
#include "quick_hull_3d.h"

using namespace renderer3d;
using namespace qh3d;


Convex3dDraw::Convex3dDraw(const std::vector<Vec3>& points)
    : points_(points)
{
    try {
        faces_ = convex_hull_3d(points_);
    } catch (const std::exception& e) {
        std::cerr << "QuickHull error: " << e.what() << std::endl;
        faces_.clear();
    }
    std::cout << "[Convex3dDraw] points=" << points_.size()
              << " faces=" << faces_.size() << std::endl;
}

bool Convex3dDraw::init(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "glfwInit failed\n";
        return false;
    }

    // Optional: set context version/profile if you plan to use modern GL
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_) {
        std::cerr << "glfwCreateWindow failed\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

    // --- IMPORTANT: initialize GL function pointers BEFORE any gl* calls ---
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    // Now it's safe to call OpenGL functions.

    // Query actual framebuffer size for correct aspect
    int fbw, fbh;
    glfwGetFramebufferSize(window_, &fbw, &fbh);
    if (fbh == 0) fbh = 1;
    double aspect = double(fbw) / double(fbh);

    // Setup projection (using glFrustum)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double fovY = 45.0;
    double zNear = 0.1;
    double zFar  = 100.0;
    double fH = tan(fovY / 360.0 * M_PI) * zNear;
    double fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

    // Some default GL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Start with filled polygons (easier to debug)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return true;
}

void Convex3dDraw::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    if (height == 0) return;
    glViewport(0, 0, width, height);

    // Update projection to match new aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = double(width) / double(height);
    double fovY = 45.0;
    double zNear = 0.1, zFar = 100.0;
    double fH = tan(fovY / 360.0 * M_PI) * zNear;
    double fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
}

void Convex3dDraw::drawHull() {
    // quick sanity checks
    if (points_.empty()) return;

    // Draw input points first (yellow) for debug
    glPointSize(6.0f);
    glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 0.0f); // yellow
        for (const auto &p : points_) {
            glVertex3f((float)p.x, (float)p.y, (float)p.z);
        }
    glEnd();

    if (faces_.empty()) {
        // nothing to draw for hull; show points only
        return;
    }

    // Validate indices (debug)
    for (size_t i = 0; i < faces_.size(); ++i) {
        const auto &f = faces_[i];
        for (int vid : f) {
            if (vid < 0 || vid >= (int)points_.size()) {
                std::cerr << "Invalid index in faces_[" << i << "]: " << vid << std::endl;
                return; // abort drawing to avoid crash
            }
        }
    }

    // Draw filled triangles (semi-transparent red)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.2f, 0.2f, 0.8f);
        for (const auto &face : faces_) {
            for (int vid : face) {
                const Vec3 &v = points_[vid];
                glVertex3f((float)v.x, (float)v.y, (float)v.z);
            }
        }
    glEnd();
    glDisable(GL_BLEND);

    // Overlay wireframe edges (black)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.2f);
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 0.0f);
        for (const auto &face : faces_) {
            for (int vid : face) {
                const Vec3 &v = points_[vid];
                glVertex3f((float)v.x, (float)v.y, (float)v.z);
            }
        }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // restore

    
}

void Convex3dDraw::renderLoop() {
    if (!window_) return;
    float angle = 0.0f;
    while(!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // camera transform: move back and rotate slowly
        glTranslatef(0.0f, -0.5f, -5.0f);
        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(angle, 0.0f, 1.0f, 0.0f);

        drawHull();

        glfwSwapBuffers(window_);
        glfwPollEvents();

        angle += 0.3f;
        if (angle > 360.0f) angle -= 360.0f;
    }

    glfwDestroyWindow(window_);
    glfwTerminate();
}
