#include "aquagba/bios_rom.hpp"
#include "aquagba/util.hpp"

using namespace aquagba;

const int ROM_SIZE = 0x4000;

BiosRom::BiosRom(const std::vector<uint8_t>& data)
{
    mRomData = data;
}

uint32_t BiosRom::Read32(uint32_t addr)
{
    if (!HasRomData())
    {
        panic("Tried to Read32 from empty BIOS ROM!");
    }
    uint32_t result = 0;
    result |= mRomData[addr];
    result |= mRomData[addr + 1] << 8;
    result |= mRomData[addr + 2] << 16;
    result |= mRomData[addr + 3] << 24;
    return result;
}

uint16_t BiosRom::Read16(uint32_t addr)
{
    if (!HasRomData())
    {
        panic("Tried to Read16 from empty BIOS ROM!");
    }
    uint16_t result = 0;
    result |= mRomData[addr];
    result |= mRomData[addr + 1] << 8;
    return result;
}

uint8_t BiosRom::Read8(uint32_t addr)
{
    if (!HasRomData())
    {
        panic("Tried to Read8 from empty BIOS ROM!");
    }
    return mRomData[addr];
}

bool BiosRom::HasRomData()
{
    return mRomData.size() == ROM_SIZE;
}