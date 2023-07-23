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
    else if (addr >= 0x4000204 && addr <= 0x4000800)
    {
        return mSystemRegs.Read32(addr);
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
    else if (addr >= 0x4000204 && addr <= 0x4000800)
    {
        return mSystemRegs.Read8(addr);
    }
    else
    {
        panic(fmt::format("Tried to read8 invalid address {:#X}", addr));
    }
}

void Bus::Write32(uint32_t addr, uint32_t val)
{
    if (addr < 0x3FFF)
    {
        panic(fmt::format("Tried to write32 to rom. addr = {:#X}", addr));
    }
    else if (addr >= 0x4000204 && addr <= 0x4000800)
    {
        return mSystemRegs.Write32(addr, val);
    }
    else
    {
        panic(fmt::format("Tried to write32 invalid address {:#X} val {:#X}", addr, val));
    }
}

void Bus::Write8(uint32_t addr, uint8_t val)
{
    if (addr < 0x3FFF)
    {
        panic(fmt::format("Tried to write8 to rom. addr = {:#X}", addr));
    }
    else if (addr >= 0x4000204 && addr <= 0x4000800)
    {
        return mSystemRegs.Write8(addr, val);
    }
    else
    {
        panic(fmt::format("Tried to write8 invalid address {:#X} val {:#X}", addr, val));
    }
}

void Bus::UpdateBiosRom(const BiosRom& new_rom)
{
    mBiosRom = new_rom;
}