add_executable(aquagba-core-tests src/main.cpp)

target_link_libraries(aquagba-core-tests PRIVATE aquagba-core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(aquagba-core-tests)