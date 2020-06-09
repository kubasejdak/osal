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

#include <osal/Thread.hpp>
#include <osal/sleep.h>
#include <osal/sleep.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <set>
#include <utility>

TEST_CASE("Thread creation and destruction in C++", "[unit][cpp][thread]")
{
    constexpr unsigned int cParam = 0xdeadbeef;
    bool launched = false;
    auto func = [&](unsigned int arg) {
        constexpr int cDelayMs = 1000;
        osalSleepMs(cDelayMs);

        REQUIRE(arg == cParam);
        launched = true;
    };

    auto func2 = [&]() {
        constexpr int cDelayMs = 1000;
        osalSleepMs(cDelayMs);

        launched = true;
    };

    SECTION("Create thread via constructor")
    {
        osal::Thread thread(func, cParam);

        auto error = thread.join();
        REQUIRE(error == OsalError::eOk);
    }

    SECTION("Create thread via start() method")
    {
        osal::Thread thread;
        auto error = thread.start(func, cParam);
        REQUIRE(error == OsalError::eOk);

        error = thread.join();
        REQUIRE(error == OsalError::eOk);
    }

    SECTION("Start thread multiple times: via constructor and via start() method")
    {
        osal::Thread thread(func, cParam);

        auto error = thread.start(func, cParam);
        REQUIRE(error == OsalError::eThreadAlreadyStarted);

        error = thread.join();
        REQUIRE(error == OsalError::eOk);

        osal::Thread thread2(func2);

        error = thread2.start(func2);
        REQUIRE(error == OsalError::eThreadAlreadyStarted);

        error = thread2.join();
        REQUIRE(error == OsalError::eOk);
    }

    SECTION("Start thread multiple times: 2 times via start() method")
    {
        osal::Thread thread;
        auto error = thread.start(func, cParam);
        REQUIRE(error == OsalError::eOk);

        error = thread.start(func, cParam);
        REQUIRE(error == OsalError::eThreadAlreadyStarted);

        error = thread.join();
        REQUIRE(error == OsalError::eOk);
    }

    REQUIRE(launched);
}

TEST_CASE("Thread creation with custom stack", "[unit][cpp][thread]")
{
    bool launched = false;
    auto func = [&]() {
        constexpr int cDelayMs = 1000;
        osalSleepMs(cDelayMs);

        launched = true;
    };

    SECTION("Create thread with custom stack")
    {
        std::array<char, cOsalThreadDefaultStackSize> stack{};
        osal::Thread thread;

        auto error = thread.setStack(stack.data());
        REQUIRE(error == OsalError::eOk);

        error = thread.start(func);
        REQUIRE(error == OsalError::eOk);

        error = thread.join();
        REQUIRE(error == OsalError::eOk);

        REQUIRE(launched);
    }

    SECTION("Create thread with nullptr stack")
    {
        osal::Thread thread;
        auto error = thread.setStack(nullptr);
        REQUIRE(error == OsalError::eInvalidArgument);
    }
}

TEST_CASE("Thread creation with variadic arguments", "[unit][cpp][thread]")
{
    bool launched = false;
    int counter = 0;
    int expectedCounter = 0;

    SECTION("No arguments")
    {
        auto func = [&] {
            osal::sleep(100ms);
            launched = true;
        };

        osal::Thread thread(func);
    }

    SECTION("1 argument")
    {
        auto func = [&](int a) {
            osal::sleep(100ms);
            counter += a;
            launched = true;
        };

        osal::Thread thread(func, 1);
        expectedCounter = 1;
    }

    SECTION("7 arguments")
    {
        auto func = [&](int a, int b, int c, int d, int e, int f, int g) {
            osal::sleep(100ms);
            counter += (a + b + c + d + e + f + g);
            launched = true;
        };

        osal::Thread thread(func, 1, 2, 3, 4, 1, 2, 3);
        constexpr int cExpectedCounter = 16;
        expectedCounter = cExpectedCounter;
    }

    REQUIRE(launched);
    REQUIRE(counter == expectedCounter);
}

TEST_CASE("Thread creation in C++ with different priorities", "[unit][cpp][thread]")
{
    bool launched = false;

    auto func = [&] {
        osal::sleep(100ms);
        launched = true;
    };

    SECTION("eLowest priority") { osal::Thread<OsalThreadPriority::eLowest> thread(func); }

    SECTION("eLow priority") { osal::Thread<OsalThreadPriority::eLow> thread(func); }

    SECTION("eNormal priority") { osal::Thread<OsalThreadPriority::eNormal> thread(func); }

    SECTION("eHigh priority") { osal::Thread<OsalThreadPriority::eHigh> thread(func); }

    SECTION("eHighest priority") { osal::Thread<OsalThreadPriority::eHighest> thread(func); }

    REQUIRE(launched);
}

TEST_CASE("Thread creation in C++ with different priorities using helper types", "[unit][cpp][thread]")
{
    bool launched = false;

    auto func = [&] {
        osal::sleep(100ms);
        launched = true;
    };

    SECTION("eLowest priority") { osal::LowestPrioThread<> thread(func); }

    SECTION("eLow priority") { osal::LowPrioThread<> thread(func); }

    SECTION("eNormal priority") { osal::NormalPrioThread<> thread(func); }

    SECTION("eHigh priority") { osal::HighPrioThread<> thread(func); }

    SECTION("eHighest priority") { osal::HighestPrioThread<> thread(func); }

    REQUIRE(launched);
}

TEST_CASE("Move thread around", "[unit][cpp][thread]")
{
    constexpr unsigned int cParam = 0xdeadbeef;
    bool launched = false;
    auto func = [&](unsigned int arg) {
        constexpr int cDelayMs = 5000;
        osalSleepMs(cDelayMs);

        REQUIRE(arg == cParam);
        launched = true;
    };

    osal::Thread thread1(func, cParam);
    osal::Thread thread2(std::move(thread1));

    auto error = thread1.join(); // NOLINT
    REQUIRE(error == OsalError::eInvalidArgument);

    error = thread2.join();
    REQUIRE(error == OsalError::eOk);

    REQUIRE(launched);
}

TEST_CASE("Multiple thread joins in C++", "[unit][cpp][thread]")
{
    auto func = [] {};

    osal::Thread thread(func);

    auto error = thread.join();
    REQUIRE(error == OsalError::eOk);

    error = thread.join();
    REQUIRE(error == OsalError::eOsError);
}

TEST_CASE("Join invalid thread in C++", "[unit][cpp][thread]")
{
    osal::Thread thread;

    auto error = thread.join();
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Launch 5 threads in C++ and check their results", "[unit][cpp][thread]")
{
    constexpr int cIterationsCount = 100;
    auto func = [](int& counter) {
        for (counter = 0; counter < cIterationsCount; ++counter)
            ;
    };

    constexpr std::size_t cThreadsCount = 5;
    std::array<osal::Thread<>, cThreadsCount> threads{};
    std::array<int, cThreadsCount> counters{};

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto error = threads[i].start(func, std::ref(counters[i]));
        REQUIRE(error == OsalError::eOk);
    }

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto error = threads[i].join();
        REQUIRE(error == OsalError::eOk);

        auto counter = counters[i];
        REQUIRE(counter == cIterationsCount);
    }
}

TEST_CASE("Launch 5 threads in C++ with different priorities and check their results", "[unit][cpp][thread]")
{
    constexpr std::size_t cThreadsCount = 5;
    std::array<unsigned int, cThreadsCount> counters{};
    bool start{};
    bool stop{};

    auto func = [](unsigned int& counter, bool& start, bool& stop) {
        while (!start)
            osal::thread::yield();

        while (!stop) {
            ++counter;
            osal::thread::yield();
        }
    };

    osal::LowestPrioThread<> thread1(func, std::ref(counters[0]), std::ref(start), std::ref(stop));
    osal::LowestPrioThread<> thread2(func, std::ref(counters[1]), std::ref(start), std::ref(stop));
    osal::LowPrioThread<> thread3(func, std::ref(counters[2]), std::ref(start), std::ref(stop));
    osal::LowPrioThread<> thread4(func, std::ref(counters[3]), std::ref(start), std::ref(stop));
    osal::NormalPrioThread<> thread5(func, std::ref(counters[4]), std::ref(start), std::ref(stop));

    start = true; // NOLINT
    osal::sleep(5s);
    stop = true; // NOLINT

    auto error = thread1.join();
    REQUIRE(error == OsalError::eOk);
    error = thread2.join();
    REQUIRE(error == OsalError::eOk);
    error = thread3.join();
    REQUIRE(error == OsalError::eOk);
    error = thread4.join();
    REQUIRE(error == OsalError::eOk);
    error = thread5.join();
    REQUIRE(error == OsalError::eOk);

    for (auto counter : counters)
        REQUIRE(counter != 0);
}

TEST_CASE("Check if thread ids are unique and constant in C++", "[unit][cpp][thread]")
{
    constexpr std::size_t cThreadsCount = 5;
    std::array<std::uint32_t, cThreadsCount> ids{};
    bool start{};

    auto func = [](std::uint32_t& id, bool& start) {
        while (!start)
            osal::thread::yield();

        id = osal::thread::id();
        constexpr int cIterationsCount = 1000;
        for (int i = 0; i < cIterationsCount; ++i) {
            auto tmpId = osal::thread::id();
            if (tmpId != id)
                REQUIRE(tmpId == id);

            osal::thread::yield();
        }
    };

    osal::LowestPrioThread<> thread1(func, std::ref(ids[0]), std::ref(start));
    osal::LowestPrioThread<> thread2(func, std::ref(ids[1]), std::ref(start));
    osal::LowPrioThread<> thread3(func, std::ref(ids[2]), std::ref(start));
    osal::LowPrioThread<> thread4(func, std::ref(ids[3]), std::ref(start));
    osal::NormalPrioThread<> thread5(func, std::ref(ids[4]), std::ref(start));

    start = true; // NOLINT

    auto error = thread1.join();
    REQUIRE(error == OsalError::eOk);
    error = thread2.join();
    REQUIRE(error == OsalError::eOk);
    error = thread3.join();
    REQUIRE(error == OsalError::eOk);
    error = thread4.join();
    REQUIRE(error == OsalError::eOk);
    error = thread5.join();
    REQUIRE(error == OsalError::eOk);

    std::set<std::uint32_t> uniqueIds;
    for (auto id : ids)
        uniqueIds.insert(id);

    REQUIRE(uniqueIds.size() == cThreadsCount);
}
