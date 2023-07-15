#pragma once

#include <string>
#include "fmt/core.h"

namespace aquagba
{

// TODO: Do something else. This is bad
[[noreturn]] static void panic(const std::string& msg)
{
    fmt::println("Panic! Reason = {}", msg);
    fmt::println("Exiting...");
    exit(1);
}

template <typename T>
static bool GetBit(const T value, const int index)
{
    return static_cast<bool>((value >> index) & 0x1);
}

// From http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
template <typename T, unsigned B>
static inline T SignExtend(const T x)
{
  struct {T x:B;} s;
  return s.x = x;
}

}