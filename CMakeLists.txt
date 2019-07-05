# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project ("typed_count")

# Include sub-projects.
add_subdirectory ("typed_count")
