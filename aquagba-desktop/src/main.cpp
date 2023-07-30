#include "fmt/core.h"
#include "aquagba/emu.hpp"
#include "aquagba/gamepak.hpp"
#include <vector>
#include <stdint.h>
#include <fstream>

// Don't mind me, just leaking some of my path info here :)
// TODO: Not this
const std::string DEFAULT_BIOS_PATH = "/Users/colin/Development/cpp/aquagba/aquagba-desktop/gba_bios.bin";

int main()
{
    aquagba::EmuCore emu_core;
    fmt::println("Starting aquagba version {}",emu_core.GetVersion());

    // Read in bios data
    std::ifstream stream(DEFAULT_BIOS_PATH, std::ios::in | std::ios::binary);
    if (!stream.is_open())
    {
        fmt::println("Failed to open bios file at '{}'! Exiting...", DEFAULT_BIOS_PATH);
        exit(1);
    }
    std::vector<uint8_t> bios_data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    emu_core.LoadBiosData(bios_data);

    // Read in gamepak data
    std::ifstream gamepak_stream("/Users/colin/Development/cpp/aquagba/aquagba-desktop/armwrestler-gba-fixed.gba", std::ios::in | std::ios::binary);
    if (!stream.is_open())
    {
        fmt::println("Failed to open gba file at '{}'! Exiting...", DEFAULT_BIOS_PATH);
        exit(1);
    }
    std::vector<uint8_t> gamepak_data((std::istreambuf_iterator<char>(gamepak_stream)), std::istreambuf_iterator<char>());


    emu_core.InsertGamepak(aquagba::Gamepak{gamepak_data}, true);

    while (true)
    {
        //auto frame = emu_core.ProcessFrame();
        //displayFrame();
        emu_core.StepClockCycle();
    }
}