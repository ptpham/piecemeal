cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-std=c++11 -g -O3")

include_directories(inc dep/ptest)
add_library(piecemeal src/logic src/dag src/compile src/preprocess
  src/game src/prop src/ioaux)

add_executable(test_unit dep/ptest/ptest_main
  unit/logic unit/compile unit/game unit/preprocess unit/dag)
target_link_libraries(test_unit piecemeal)

add_executable(cps app/cps)
target_link_libraries(cps piecemeal)

add_executable(play app/play)
target_link_libraries(play piecemeal)

add_executable(piecelog app/piecelog)
target_link_libraries(piecelog piecemeal)

add_executable(piecemill app/piecemill)
target_link_libraries(piecemill piecemeal)
