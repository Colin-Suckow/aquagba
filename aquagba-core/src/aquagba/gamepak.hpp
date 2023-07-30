#pragma once

#include <vector>
#include <stdint.h>

namespace aquagba
{
class Gamepak
{
public:
    Gamepak(const std::vector<uint8_t>& data);

    uint32_t Read32(uint32_t addr);
private:
    std::vector<uint8_t> mData;
};
}