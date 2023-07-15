#include "aquagba/emu.hpp"
#include <string>

aquagba::EmuCore::EmuCore():
mCpu(),
mBus(BiosRom{{}}) // Start with empty bios rom
{

}

std::string aquagba::EmuCore::GetVersion()
{
    return AQUAGBA_CORE_VERSION_STRING;
}

void aquagba::EmuCore::LoadBiosData(const std::vector<uint8_t>& data)
{
    mBus.UpdateBiosRom(BiosRom{data});
}

void aquagba::EmuCore::StepClockCycle()
{
    mCpu.RunNextInstruction(mBus);
}