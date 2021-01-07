/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2020-2021, Kuba Sejdak <kuba.sejdak@gmail.com>
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

#include <osal/Thread.h>
#include <osal/sleep.hpp>

#include <catch2/catch.hpp>

#include <array>
#include <set>

TEST_CASE("Thread creation and destruction", "[unit][c][thread]")
{
    auto func = [](void* /*unused*/) {};

    OsalThread thread{};
    auto error
        = osalThreadCreate(&thread, {cOsalThreadDefaultPriority, cOsalThreadDefaultStackSize, nullptr}, func, nullptr);
    REQUIRE(error == OsalError::eOk);

    error = osalThreadJoin(&thread);
    REQUIRE(error == OsalError::eOk);

    error = osalThreadDestroy(&thread);
    REQUIRE(error == OsalError::eOk);

    error = osalThreadDestroy(&thread);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Thread creation with invalid arguments", "[unit][c][thread]")
{
    auto func = [](void* /*unused*/) {};

    OsalThread thread{};
    auto error
        = osalThreadCreate(nullptr, {cOsalThreadDefaultPriority, cOsalThreadDefaultStackSize, nullptr}, func, nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalThreadCreate(&thread,
                             {cOsalThreadDefaultPriority, cOsalThreadDefaultStackSize, nullptr},
                             nullptr,
                             nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    constexpr int cInvalidPriority = 5;
    error = osalThreadCreate(&thread,
                             {static_cast<OsalThreadPriority>(cInvalidPriority), cOsalThreadDefaultStackSize, nullptr},
                             func,
                             nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Multiple thread joins", "[unit][c][thread]")
{
    auto func = [](void* /*unused*/) {};

    OsalThread thread{};
    auto error
        = osalThreadCreate(&thread, {cOsalThreadDefaultPriority, cOsalThreadDefaultStackSize, nullptr}, func, nullptr);
    REQUIRE(error == OsalError::eOk);

    error = osalThreadJoin(&thread);
    REQUIRE(error == OsalError::eOk);

    error = osalThreadJoin(&thread);
    REQUIRE(error == OsalError::eOsError);

    error = osalThreadDestroy(&thread);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Join invalid thread", "[unit][c][thread]")
{
    OsalThread thread{};

    auto error = osalThreadJoin(&thread);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalThreadJoin(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Launch 5 threads and check their results", "[unit][c][thread]")
{
    constexpr int cIterationsCount = 100;
    constexpr std::size_t cThreadsCount = 5;
    std::array<OsalThread, cThreadsCount> threads{};
    std::array<int, cThreadsCount> counters{};

    auto func = [](void* arg) {
        auto& counter = *static_cast<int*>(arg);
        for (counter = 0; counter < cIterationsCount; ++counter)
            ;
    };

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto error = osalThreadCreate(&threads[i],
                                      {cOsalThreadDefaultPriority, cOsalThreadDefaultStackSize, nullptr},
                                      func,
                                      &counters[i]);
        REQUIRE(error == OsalError::eOk);
    }

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto error = osalThreadJoin(&threads[i]);
        REQUIRE(error == OsalError::eOk);

        auto counter = counters[i];
        REQUIRE(counter == cIterationsCount);

        error = osalThreadDestroy(&threads[i]);
        REQUIRE(error == OsalError::eOk);
    }
}

TEST_CASE("Launch 5 threads with different priorities and check their results", "[unit][c][thread]")
{
    using ThreadArgs = std::tuple<unsigned int&, bool&, bool&>;

    constexpr std::size_t cThreadsCount = 5;
    std::array<OsalThread, cThreadsCount> threads{};
    std::array<unsigned int, cThreadsCount> counters{};
    bool start{};
    bool stop{};

    auto func = [](void* arg) {
        auto& params = *static_cast<ThreadArgs*>(arg);
        auto& counter = std::get<0>(params);
        auto& start = std::get<1>(params);
        auto& stop = std::get<2>(params);

        while (!start)
            osalThreadYield();

        while (!stop) {
            ++counter;
            osalThreadYield();
        }
    };

    std::array<ThreadArgs, cThreadsCount> args
        = {std::make_tuple(std::ref(counters[0]), std::ref(start), std::ref(stop)),
           std::make_tuple(std::ref(counters[1]), std::ref(start), std::ref(stop)),
           std::make_tuple(std::ref(counters[2]), std::ref(start), std::ref(stop)),
           std::make_tuple(std::ref(counters[3]), std::ref(start), std::ref(stop)),
           std::make_tuple(std::ref(counters[4]), std::ref(start), std::ref(stop))};

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto priority = static_cast<OsalThreadPriority>(i / 2);
        auto error = osalThreadCreate(&threads[i], {priority, cOsalThreadDefaultStackSize, nullptr}, func, &args[i]);
        REQUIRE(error == OsalError::eOk);
    }

    start = true; // NOLINT
    osal::sleep(5s);
    stop = true; // NOLINT

    for (auto& thread : threads) {
        auto error = osalThreadJoin(&thread);
        REQUIRE(error == OsalError::eOk);

        error = osalThreadDestroy(&thread);
        REQUIRE(error == OsalError::eOk);
    }

    for (auto counter : counters)
        REQUIRE(counter != 0);
}

TEST_CASE("Create threads with all priorities", "[unit][c][thread]")
{
    auto func = [](void* /*unused*/) { osal::sleep(1s); };

    for (int i = OsalThreadPriority::eLowest; i <= OsalThreadPriority::eHighest; ++i) {
        OsalThread thread{};
        auto priority = static_cast<OsalThreadPriority>(i / 2);
        auto error = osalThreadCreate(&thread, {priority, cOsalThreadDefaultStackSize, nullptr}, func, nullptr);
        REQUIRE(error == OsalError::eOk);

        error = osalThreadJoin(&thread);
        REQUIRE(error == OsalError::eOk);

        error = osalThreadDestroy(&thread);
        REQUIRE(error == OsalError::eOk);
    }
}

TEST_CASE("Check if thread ids are unique and constant", "[unit][c][thread]")
{
    using ThreadArgs = std::tuple<std::uint32_t&, bool&>;

    constexpr std::size_t cThreadsCount = 5;
    std::array<OsalThread, cThreadsCount> threads{};
    std::array<std::uint32_t, cThreadsCount> ids{};
    bool start{};

    auto func = [](void* arg) {
        auto& params = *static_cast<ThreadArgs*>(arg);
        auto& id = std::get<0>(params);
        auto& start = std::get<1>(params);

        while (!start)
            osalThreadYield();

        id = osalThreadId();
        constexpr int cIterationsCount = 1000;
        for (int i = 0; i < cIterationsCount; ++i) {
            auto tmpId = osalThreadId();
            if (tmpId != id)
                REQUIRE(tmpId == id);

            osalThreadYield();
        }
    };

    std::array<ThreadArgs, cThreadsCount> args = {std::make_tuple(std::ref(ids[0]), std::ref(start)),
                                                  std::make_tuple(std::ref(ids[1]), std::ref(start)),
                                                  std::make_tuple(std::ref(ids[2]), std::ref(start)),
                                                  std::make_tuple(std::ref(ids[3]), std::ref(start)),
                                                  std::make_tuple(std::ref(ids[4]), std::ref(start))};

    for (std::size_t i = 0; i < threads.size(); ++i) {
        auto priority = static_cast<OsalThreadPriority>(i / 2);
        auto error = osalThreadCreate(&threads[i], {priority, cOsalThreadDefaultStackSize, nullptr}, func, &args[i]);
        REQUIRE(error == OsalError::eOk);
    }

    start = true; // NOLINT

    for (auto& thread : threads) {
        auto error = osalThreadJoin(&thread);
        REQUIRE(error == OsalError::eOk);

        error = osalThreadDestroy(&thread);
        REQUIRE(error == OsalError::eOk);
    }

    std::set<std::uint32_t> uniqueIds;
    for (auto id : ids)
        uniqueIds.insert(id);

    REQUIRE(uniqueIds.size() == cThreadsCount);
}
