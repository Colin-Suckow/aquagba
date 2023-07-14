#pragma once

#include <string>
#include <vector>

namespace aquagba
{
class EmuCore
{
public:
    EmuCore();
    std::string GetVersion();
    void LoadBiosData(const std::vector<uint8_t>& data);
    void StepClockCycle();
};
}