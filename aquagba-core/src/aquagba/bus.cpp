#include "aquagba/bus.hpp"
#include "stdint.h"
#include <iostream>
#include "aquagba/util.hpp"
#include <fmt/format.h>

using namespace aquagba;

Bus::Bus(const BiosRom& rom):
mBiosRom(rom)
{

}

uint32_t Bus::Read32(uint32_t addr)
{
    if (addr < 0x3FFF)
    {
        return mBiosRom.Read32(addr);
    }
    else
    {
        panic(fmt::format("Tried to read32 invalid address {:#X}", addr));
    }
}

uint16_t Bus::Read16(uint32_t addr)
{
    if (addr < 0x3FFF)
    {
        return mBiosRom.Read16(addr);
    }
    else
    {
        panic(fmt::format("Tried to read16 invalid address {:#X}", addr));
    }
}

uint8_t Bus::Read8(uint32_t addr)
{
    if (addr < 0x3FFF)
    {
        return mBiosRom.Read8(addr);
    }
    else
    {
        panic(fmt::format("Tried to read8 invalid address {:#X}", addr));
    }
}

void Bus::UpdateBiosRom(const BiosRom& new_rom)
{
    mBiosRom = new_rom;
}