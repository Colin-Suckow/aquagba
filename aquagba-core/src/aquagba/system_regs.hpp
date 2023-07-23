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

private:
    uint32_t mPostflag;
};
}