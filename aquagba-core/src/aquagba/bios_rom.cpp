#include "aquagba/bios_rom.hpp"

aquagba::BiosRom::BiosRom(const std::vector<uint8_t>& data)
{
    mRomData = data;
}