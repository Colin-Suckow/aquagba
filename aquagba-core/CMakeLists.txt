add_library(
    aquagba-core
    src/aquagba/emu.hpp
    src/aquagba/emu.cpp
    src/aquagba/bios_rom.hpp
    src/aquagba/bios_rom.cpp
    src/aquagba/Arm7Tdmi.hpp
    src/aquagba/Arm7Tdmi.cpp
)

target_include_directories(aquagba-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_definitions(aquagba-core PUBLIC AQUAGBA_CORE_VERSION_STRING="0.1")