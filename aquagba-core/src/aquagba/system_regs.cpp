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

uint8_t SystemRegs::Read8(uint32_t addr)
{
    switch(addr)
    {
    case 0x4000300:
        return mPostflag & 0xF;
    default:
        panic(fmt::format("Unknown system register addr during read8. addr = {:#X}", addr));
    }
}

void SystemRegs::Write8(uint32_t addr, uint8_t val)
{
    switch(addr)
    {
    case 0x4000208:
        mMasterIrqControl = val;
        break;
    default:
        panic(fmt::format("Unknown system register addr during write8. addr = {:#X}, val = {:#X}", addr, val));
    }
}

bool SystemRegs::AllIrqsDisabled()
{
    return !GetBit(mMasterIrqControl, 0);
}