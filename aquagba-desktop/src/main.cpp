#include <iostream>
#include "aquagba/emu.hpp"
#include <vector>
#include <stdint.h>

int main()
{
    aquagba::EmuCore emu_core;
    std::cout << "Starting aquagba version " << emu_core.GetVersion() << std::endl;

    std::vector<uint8_t> bios_data;
    emu_core.LoadBiosData(bios_data);

    //emu_core.InsertGamePak();

    while (true)
    {
        //auto frame = emu_core.ProcessFrame();
        //displayFrame();
        emu_core.StepClockCycle();
    }
}