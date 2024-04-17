cmake_minimum_required(VERSION 3.10)

project(rays C CXX)

add_subdirectory(lib/raylib)

add_executable(drag_drop drag_drop.c)
target_link_libraries(drag_drop PRIVATE raylib)

add_executable(minesweeper minesweeper.c)
target_link_libraries(minesweeper PRIVATE raylib)

add_executable(charge charge.c)
target_link_libraries(charge PRIVATE raylib)

add_executable(graph graph.cpp)
target_link_libraries(graph PRIVATE raylib)
