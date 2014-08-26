cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=c++1y -g")

include_directories(inc dep/ptest)
add_library(piecemeal src/logic.cpp src/dag.cpp src/compile.cpp src/gdl.cpp)

add_executable(test_unit dep/ptest/ptest_main.cpp unit/logic.cpp)
target_link_libraries(test_unit piecemeal)

