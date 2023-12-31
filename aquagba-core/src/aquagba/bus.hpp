#pragma once

#include <stdint.h>
#include <optional>

#include "aquagba/bios_rom.hpp"
#include "aquagba/system_regs.hpp"
#include "aquagba/gamepak.hpp"

namespace aquagba
{

class Bus
{
public:
    Bus(const BiosRom& rom);

    uint32_t Read32(uint32_t addr);
    uint16_t Read16(uint32_t addr);
    uint8_t Read8(uint32_t addr);

    void Write32(uint32_t addr, uint32_t val);
    void Write8(uint32_t addr, uint8_t val);

    void UpdateBiosRom(const BiosRom& new_rom);
    void InsertGamepak(const Gamepak& gamepak);
    void RemoveGamepak();
private:
    BiosRom mBiosRom;
    SystemRegs mSystemRegs;
    std::optional<Gamepak> mGamepak;
};

}