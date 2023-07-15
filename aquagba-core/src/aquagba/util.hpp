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

}