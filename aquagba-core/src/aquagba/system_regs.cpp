#include "aquagba/system_regs.hpp"
#include "aquagba/util.hpp"
#include "fmt/format.h"
#include <stdint.h>

using namespace aquagba;

SystemRegs::SystemRegs()
{
    mPostflag = 0;
}

uint32_t SystemRegs::Read32(uint32_t addr)
{
    switch(addr)
    {
    case 0x4000300:
        return mPostflag;
    default:
        panic(fmt::format("Unknown system register addr during read32. addr = {:#X}", addr));
    }
}

void SystemRegs::Write32(uint32_t addr, uint32_t val)
{
    switch(addr)
    {
    default:
        panic(fmt::format("Unknown system register addr during write32. addr = {:#X}, val = {:#X}", addr, val));
    }
}