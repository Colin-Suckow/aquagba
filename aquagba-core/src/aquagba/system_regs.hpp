#pragma once

#include <stdint.h>

namespace aquagba
{
class SystemRegs
{
public:
    SystemRegs();
    
    uint32_t Read32(uint32_t addr);
    void Write32(uint32_t addr, uint32_t val);

    uint8_t Read8(uint32_t addr);
    void Write8(uint32_t addr, uint8_t val);

    bool AllIrqsDisabled();

private:
    uint32_t mPostflag;
    uint32_t mMasterIrqControl;
};
}