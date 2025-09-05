

cmake_minimum_required(VERSION 3.10)
project(ConvexHull)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Define cpp files
file(GLOB SOURCES src/*.cpp)

add_executable(convex_hull ${SOURCES})

# Link with SFML
target_link_libraries(convex_hull PRIVATE sfml-graphics sfml-window sfml-system)