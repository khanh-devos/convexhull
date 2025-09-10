

cmake_minimum_required(VERSION 3.10)
project(ConvexHull)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(
    include
)


# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# GLFW
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(ALGO_SOURCES
    src/quick_hull.cpp       # algorithm implementation(s)
    src/draw.cpp
    src/graham_hull.cpp
    src/quick_hull_3d.cpp
    src/draw3d.cpp
    src/glad.c
    # add other algorithm .cpp files here, but NOT main.cpp
)

set(MAIN_SOURCE
    src/main.cpp            # your SFML visualization entry point
)

# ---- Library with algorithms (no main) ----
add_library(convexhull_lib ${ALGO_SOURCES})
target_include_directories(convexhull_lib PUBLIC include)

# ---- Main demo executable ----
add_executable(convex_hull ${MAIN_SOURCE})
target_link_libraries(convex_hull PRIVATE 
    convexhull_lib 
    sfml-graphics 
    sfml-window 
    sfml-system
    #----3d----
    glfw
    dl
    OpenGL::GL
    X11
    pthread    
)

# ---- Google Test ----

enable_testing()
find_package(GTest REQUIRED)

# Unit tests
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests PRIVATE convexhull_lib GTest::GTest GTest::Main)

include(GoogleTest)
add_test(NAME unit_tests COMMAND unit_tests)
