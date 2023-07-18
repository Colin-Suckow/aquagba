cmake_minimum_required(VERSION 3.12)
project(AquaGBA VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 20)

add_subdirectory(lib/fmt)
add_subdirectory(lib/Catch2)

add_subdirectory(aquagba-core)
add_subdirectory(aquagba-desktop)
add_subdirectory(aquagba-core-tests)