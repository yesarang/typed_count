# CMakeList.txt : CMake project for typed_count, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories(include)

# Add source to this project's executable.
file(GLOB SOURCES "src/*.cpp")

add_executable (typed_count ${SOURCES})

