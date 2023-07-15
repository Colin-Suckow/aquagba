#pragma once

#include <string>
#include <vector>
#include "aquagba/Arm7Tdmi.hpp"

namespace aquagba
{
class EmuCore
{
public:
    EmuCore();
    std::string GetVersion();
    void LoadBiosData(const std::vector<uint8_t>& data);
    void StepClockCycle();
private:
    Arm7Tdmi mCpu;
    Bus mBus;
};
}