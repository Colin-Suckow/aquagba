#pragma once

#include <vector>
#include <stdint.h>

namespace aquagba{
class BiosRom
{
public:
    BiosRom(const std::vector<uint8_t>& data);

    uint32_t Read32(uint32_t addr);
private:
    std::vector<uint8_t> mRomData;
};
}