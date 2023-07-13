add_library(
    aquagba-core
    src/emu.hpp
    src/emu.cpp
)

target_include_directories(aquagba-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_definitions(aquagba-core PUBLIC AQUAGBA_CORE_VERSION=1)