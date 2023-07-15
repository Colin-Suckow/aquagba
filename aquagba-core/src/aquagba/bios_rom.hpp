#pragma once

#include <vector>
#include <stdint.h>

namespace aquagba{
class BiosRom
{
public:
    BiosRom(const std::vector<uint8_t>& data);

    uint32_t Read32(uint32_t addr);
    uint16_t Read16(uint32_t addr);
    uint8_t Read8(uint32_t addr);
private:
    bool HasRomData();
    std::vector<uint8_t> mRomData;
};
}