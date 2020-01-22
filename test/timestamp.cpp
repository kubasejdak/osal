/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2020-2020, Kuba Sejdak <kuba.sejdak@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this
///    list of conditions and the following disclaimer.
///
/// 2. Redistributions in binary form must reproduce the above copyright notice,
///    this list of conditions and the following disclaimer in the documentation
///    and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
/// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
/// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
/// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
/// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
/// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/////////////////////////////////////////////////////////////////////////////////////

#include <osal/sleep.h>
#include <osal/timestamp.h>

#include <catch2/catch.hpp>

#include <cstdint>

TEST_CASE("Check timestamp values after multiple delays", "[c][unit][timestamp]")
{
    std::uint64_t delayMs{};

    SECTION("Delay 500ms")
    {
        constexpr std::uint64_t cDelayMs = 500;
        delayMs = cDelayMs;
    }

    SECTION("Delay 1ms")
    {
        constexpr std::uint64_t cDelayMs = 1;
        delayMs = cDelayMs;
    }

    SECTION("Delay 0ms")
    {
        constexpr std::uint64_t cDelayMs = 0;
        delayMs = cDelayMs;
    }

    auto now1 = osalTimestampGetMs();
    osalSleepMs(delayMs);
    auto now2 = osalTimestampGetMs();
    const auto delay2Ms = 2 * delayMs;
    osalSleepMs(delay2Ms);
    auto now3 = osalTimestampGetMs();

    REQUIRE(now2 >= now1);
    REQUIRE((now2 - now1) >= delayMs);
    REQUIRE(now3 >= now2);
    REQUIRE(now3 >= now1);
    REQUIRE((now3 - now2) >= delay2Ms);
    REQUIRE((now3 - now1) >= (delay2Ms + delayMs));
}
