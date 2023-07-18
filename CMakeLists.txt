cmake_minimum_required(VERSION 3.12)
project(AquaGBA VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 20)
set (LIBCXX_ENABLE_INCOMPLETE_FEATURES ON)

add_subdirectory(aquagba-core)
add_subdirectory(aquagba-desktop)
add_subdirectory(lib/fmt)