#include "aquagba/emu.hpp"
#include <string>

aquagba::EmuCore::EmuCore()
{

}

std::string aquagba::EmuCore::GetVersion()
{
    return AQUAGBA_CORE_VERSION_STRING;
}

void aquagba::EmuCore::LoadBiosData(const std::vector<uint8_t>& data)
{

}

void aquagba::EmuCore::StepClockCycle()
{
    
}