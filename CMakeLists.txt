cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=c++1y -g")

include_directories(inc dep/ptest)
add_library(piecemeal src/logic src/dag src/compile src/preprocess src/game)

add_executable(test_unit dep/ptest/ptest_main
  unit/logic unit/compile unit/game unit/preprocess unit/dag)
target_link_libraries(test_unit piecemeal)

add_executable(play src/play.cpp)
target_link_libraries(play piecemeal)

