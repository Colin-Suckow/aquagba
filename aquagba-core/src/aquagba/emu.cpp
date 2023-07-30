#include "aquagba/emu.hpp"
#include <string>

using namespace aquagba;

EmuCore::EmuCore():
mCpu(),
mBus(BiosRom{{}}) // Start with empty bios rom
{

}

std::string EmuCore::GetVersion()
{
    return AQUAGBA_CORE_VERSION_STRING;
}

void EmuCore::LoadBiosData(const std::vector<uint8_t>& data)
{
    mBus.UpdateBiosRom(BiosRom{data});
}

void EmuCore::StepClockCycle()
{
    mCpu.RunNextInstruction(mBus);
}

void EmuCore::InsertGamepak(const Gamepak gamepak, bool forceJump)
{
    mBus.InsertGamepak(gamepak);

    if (forceJump)
    {
        mCpu.GetRegisterDirect(RegisterName::r15) = 0x08000000;
    }
}