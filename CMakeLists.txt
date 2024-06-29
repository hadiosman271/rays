cmake_minimum_required(VERSION 3.10)

project(rays C)

if (NOT DEFINED RAYLIB_PATH)
	message(FATAL_ERROR "RAYLIB_PATH not defined")
endif (NOT DEFINED RAYLIB_PATH)

add_subdirectory(${RAYLIB_PATH} ${RAYLIB_PATH}/build)

add_executable(drag_drop drag_drop.c)
target_link_libraries(drag_drop PRIVATE raylib)

add_executable(minesweeper minesweeper.c)
target_link_libraries(minesweeper PRIVATE raylib)

add_executable(charge charge.c)
target_link_libraries(charge PRIVATE raylib)

add_executable(coords coords.c)
target_link_libraries(coords PRIVATE raylib)
