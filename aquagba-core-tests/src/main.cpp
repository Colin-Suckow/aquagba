#include <catch2/catch_test_macros.hpp>
#include <aquagba/emu.hpp>

using namespace aquagba;

TEST_CASE("Version method returns the correct version", "[emu]" )
{
    EmuCore emu{};

    REQUIRE(emu.GetVersion() == AQUAGBA_CORE_VERSION_STRING);
}