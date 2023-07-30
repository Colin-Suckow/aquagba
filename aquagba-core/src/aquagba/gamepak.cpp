#include "aquagba/gamepak.hpp"
#include "aquagba/util.hpp"

#include <stdint.h>
#include <vector>

using namespace aquagba;

const uint32_t ADDR_OFFSET = 0x08000000;

Gamepak::Gamepak(const std::vector<uint8_t>& data)
{
    mData = data;
}

uint32_t Gamepak::Read32(uint32_t addr)
{
    if (addr < ADDR_OFFSET || addr > ADDR_OFFSET + mData.size()) panic("Bad cart read :(");

    uint32_t index = mData[addr - ADDR_OFFSET];
    uint32_t result = 0;
    result |= mData[index];
    result |= mData[index + 1] << 8;
    result |= mData[index + 2] << 16;
    result |= mData[index + 3] << 24;
    return result;
}