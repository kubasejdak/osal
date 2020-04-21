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

#include <osal/Timeout.hpp>
#include <osal/sleep.hpp>
#include <osal/timestamp.hpp>

#include <catch2/catch.hpp>

#include <algorithm>

TEST_CASE("Creation of timeout", "[unit][cpp][timeout]")
{
    osal::Timeout t1(3s);
    osal::Timeout t2(4ms);
    osal::Timeout t3(1ms);
    osal::Timeout t4(8ms);
    osal::Timeout t5 = 100ms;
    osal::Timeout t6(t2);
    osal::Timeout t7 = t4;
    osal::Timeout t8(100ms, true);

    REQUIRE(t1.duration() == 3s);
    REQUIRE(t2.duration() == 4ms);
    REQUIRE(t3.duration() == 1ms);
    REQUIRE(t4.duration() == 8ms);
    REQUIRE(t5.duration() == 100ms);
    REQUIRE(t6.duration() == t2.duration());
    REQUIRE(t7.duration() == t4.duration());
    REQUIRE(t8.duration() == 100ms);

    // Make all timeouts expired.
    constexpr auto cDelay = 5s;
    osal::sleep(cDelay);

    REQUIRE(t1.isExpired());
    REQUIRE(t1.isExpired());
    REQUIRE(t1.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t1.timeLeft().count() == 0);
    REQUIRE(t1.isExpired());
    REQUIRE(t1.isExpired());
    REQUIRE(t2.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t2.timeLeft().count() == 0);
    REQUIRE(t3.isExpired());
    REQUIRE(t3.isExpired());
    REQUIRE(t3.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t3.timeLeft().count() == 0);
    REQUIRE(t4.isExpired());
    REQUIRE(t4.isExpired());
    REQUIRE(t4.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t4.timeLeft().count() == 0);
    REQUIRE(t5.isExpired());
    REQUIRE(t5.isExpired());
    REQUIRE(t5.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t5.timeLeft().count() == 0);
    REQUIRE(t6.isExpired());
    REQUIRE(t6.isExpired());
    REQUIRE(t6.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t6.timeLeft().count() == 0);
    REQUIRE(t7.isExpired());
    REQUIRE(t7.isExpired());
    REQUIRE(t7.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t7.timeLeft().count() == 0);
    REQUIRE(t8.isExpired());
    REQUIRE(t8.isExpired());
    REQUIRE(t8.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE(t8.timeLeft().count() == 0);
}

TEST_CASE("Timeout is monotonic", "[unit][cpp][timeout]")
{
    constexpr auto cTimeout = 5s;
    osal::Timeout t(cTimeout);

    std::vector<std::uint64_t> timesLeft;
    while (!t.isExpired()) {
        constexpr auto cDelay = 100ms;
        osal::sleep(cDelay);
        timesLeft.push_back(t.timeLeft().count());
    }

    REQUIRE(t.timeLeft().count() == 0);
    REQUIRE(std::is_sorted(std::rbegin(timesLeft), std::rend(timesLeft)));
}

TEST_CASE("Timeout used as a function argument", "[unit][cpp][timeout]")
{
    constexpr auto cTimeout = 250ms;
    osal::Timeout timeout = cTimeout;

    auto start = osal::timestamp();

    SECTION("Measuring time elapsed until deadline is reached")
    {
        auto func = [](osal::Timeout timeout) {
            while (!timeout.isExpired()) {
                constexpr auto cDelay = 100ms;
                osal::sleep(cDelay);
            }
        };

        func(timeout);
    }

    SECTION("Timeout is propagated deep in the callstack")
    {
        auto dummySleepFunc = [=] { osal::sleep(cTimeout + 10ms); };

        auto func1 = [=](osal::Timeout timeout) {
            dummySleepFunc();
            return timeout.isExpired();
        };

        auto func2 = [=](osal::Timeout timeout) {
            bool expired = func1(timeout);
            REQUIRE(expired);
            REQUIRE(timeout.isExpired());
            return expired;
        };

        auto func3 = [=](osal::Timeout timeout) {
            bool expired = func2(timeout);
            REQUIRE(expired);
            REQUIRE(timeout.isExpired());
            return expired;
        };

        auto func4 = [=](osal::Timeout timeout) {
            bool expired = func3(timeout);
            REQUIRE(expired);
            REQUIRE(timeout.isExpired());
            return expired;
        };

        auto func5 = [=](osal::Timeout timeout) {
            bool expired = func4(timeout);
            REQUIRE(expired);
            REQUIRE(timeout.isExpired());
            return expired;
        };

        bool expired = func5(timeout);
        REQUIRE(expired);
    }

    auto end = osal::timestamp();
    [[maybe_unused]] auto diff = (end - start).count();

    REQUIRE(timeout.isExpired());
    REQUIRE(timeout.timeLeft() == std::chrono::milliseconds::zero());
    REQUIRE((end - start) >= cTimeout);
}

// TEST_CASE("Timeout used with semaphores", "[unit][cpp][timeout]")
// {
//     osal::Semaphore semaphore(0, "semaphore");
//     osal::Timeout timeout = 100ms;

//     auto result = semaphore.timedWait(timeout);
//     REQUIRE(result == osal::eTimeout);
//     REQUIRE(timeout.isExpired());
// }

TEST_CASE("Resetting timeout", "[unit][cpp][timeout]")
{
    osal::Timeout t1 = 400ms;
    osal::Timeout t2 = 100ms;

    constexpr int cIterationsCount = 10;
    for (int i = 0; i < cIterationsCount; ++i) {
        // Make all timeouts expired.
        constexpr auto cDelay = 500ms;
        osal::sleep(cDelay);

        REQUIRE(t1.isExpired());
        REQUIRE(t2.isExpired());

        t1.reset();
        REQUIRE(!t1.isExpired());

        t2.reset();
        REQUIRE(!t2.isExpired());
    }
}
