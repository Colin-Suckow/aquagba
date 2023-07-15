#pragma once

#include <stdint.h>

#include "aquagba/bios_rom.hpp"

namespace aquagba
{

class Bus
{
public:
    Bus(const BiosRom& rom);

    uint32_t Read32(uint32_t addr);
    uint16_t Read16(uint32_t addr);
    uint8_t Read8(uint32_t addr);
    void UpdateBiosRom(const BiosRom& new_rom);
private:
    BiosRom mBiosRom;
};

}