cmake_minimum_required(VERSION 3.10)

project(rays C)

add_subdirectory(lib/raylib)
add_subdirectory(lib/raygui/projects/CMake)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

add_executable(drag_drop drag_drop.c)
target_link_libraries(drag_drop PRIVATE raylib)

add_executable(minesweeper minesweeper.c)
target_link_libraries(minesweeper PRIVATE raylib)

add_executable(charge charge.c)
target_link_libraries(charge PRIVATE raylib)

add_executable(aim_assist aim_assist.c)
target_link_libraries(aim_assist PRIVATE raylib)

add_executable(fractals fractals.c)
target_link_libraries(fractals PRIVATE raylib)

add_executable(bezier bezier.c)
target_link_libraries(bezier PRIVATE raylib raygui)

add_executable(sand sand.c)
target_link_libraries(sand PRIVATE raylib)

add_executable(layout_editor layout_editor.c)
target_link_libraries(layout_editor PRIVATE raylib)

add_executable(graphs graphs.c)
target_link_libraries(graphs PRIVATE raylib)

add_executable(coords coords.c)
target_link_libraries(coords PRIVATE raylib)
