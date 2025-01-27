# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Game VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

set(source_dir "${PROJECT_SOURCE_DIR}/src/")
# Source files
file (GLOB source_files "${source_dir}/*.cpp")

# Add executable
add_executable(${PROJECT_NAME} ${source_files})

# Link libraries
find_library(RAYLIB_LIBRARY NAMES raylib PATHS lib)
target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIBRARY} GL X11 m)
