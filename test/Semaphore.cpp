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

#include <osal/Semaphore.h>
#include <osal/Thread.hpp>
#include <osal/sleep.hpp>
#include <osal/timestamp.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Semaphore creation and destruction", "[unit][c][semaphore]")
{
    unsigned int initialValue{};

    SECTION("Initial value 0") { initialValue = 0; }

    SECTION("Initial value 1") { initialValue = 1; }

    SECTION("Initial value 15")
    {
        constexpr unsigned int cInitialValue = 15;
        initialValue = cInitialValue;
    }

    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, initialValue);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Invalid parameters to semaphore creation and destruction functions", "[unit][c][semaphore]")
{
    auto error = osalSemaphoreCreate(nullptr, 3);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreDestroy(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Wait and signal from one thread, start with 1", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Wait and signal from one thread, start with 0", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 0);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Invalid arguments passed to semaphore functions in one thread", "[unit][c][semaphore]")
{
    auto error = osalSemaphoreWait(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreTryWait(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreTryWaitIsr(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreTimedWait(nullptr, 3);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreSignal(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);

    error = osalSemaphoreSignalIsr(nullptr);
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("Combination of wait and signal calls from one thread", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 4);
    REQUIRE(error == OsalError::eOk);

    // 3.
    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 2.
    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 1.
    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 2.
    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 1.
    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 2.
    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    // 3.
    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    int count{};
    while (osalSemaphoreTryWait(&semaphore) == OsalError::eOk)
        ++count;

    REQUIRE(count == 3);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Increment semaphore to value bigger than the initial one", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 4);
    REQUIRE(error == OsalError::eOk);

    int firstCount{};
    while (osalSemaphoreTryWait(&semaphore) == OsalError::eOk)
        ++firstCount;

    constexpr int cFirstExpectedCount = 4;
    REQUIRE(firstCount == cFirstExpectedCount);

    constexpr int cSecondExpectedCount = 16;
    for (int i = 0; i < cSecondExpectedCount; ++i) {
        error = osalSemaphoreSignal(&semaphore);
        REQUIRE(error == OsalError::eOk);
    }

    int secondCount{};
    while (osalSemaphoreTryWait(&semaphore) == OsalError::eOk)
        ++secondCount;

    REQUIRE(secondCount == cSecondExpectedCount);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Wait called from two threads", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        auto error = osalSemaphoreWait(&semaphore);
        if (error != OsalError::eOk)
            REQUIRE(error == OsalError::eOk);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        error = osalSemaphoreSignal(&semaphore);
        if (error != OsalError::eOk)
            REQUIRE(error == OsalError::eOk);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    thread.join();

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("TryWait called from second thread", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        while (osalSemaphoreTryWait(&semaphore) != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        auto error = osalSemaphoreSignal(&semaphore);
        if (error != OsalError::eOk)
            REQUIRE(error == OsalError::eOk);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    thread.join();

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("TryWait and signal called from ISR", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreTryWaitIsr(&semaphore);
    REQUIRE(error == OsalError::eOk);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        while (osalSemaphoreTryWaitIsr(&semaphore) != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        auto error = osalSemaphoreSignal(&semaphore);
        if (error != OsalError::eOk)
            REQUIRE(error == OsalError::eOk);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = osalSemaphoreSignalIsr(&semaphore);
    REQUIRE(error == OsalError::eOk);

    thread.join();

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Multiple wait called one thread", "[unit][c][semaphore]")
{
    unsigned int initialValue{};

    SECTION("Initial value 0") { initialValue = 0; }

    SECTION("Initial value 4")
    {
        constexpr unsigned int cInitialValue = 4;
        initialValue = cInitialValue;
    }

    SECTION("Initial value 15")
    {
        constexpr unsigned int cInitialValue = 15;
        initialValue = cInitialValue;
    }

    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, initialValue);
    REQUIRE(error == OsalError::eOk);

    for (unsigned int i = 0; i < initialValue; ++i) {
        error = osalSemaphoreWait(&semaphore);
        REQUIRE(error == OsalError::eOk);
    }

    error = osalSemaphoreTryWait(&semaphore);
    REQUIRE(error == OsalError::eLocked);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Multiple tryWait called one thread", "[unit][c][semaphore]")
{
    unsigned int initialValue{};

    SECTION("Initial value 0") { initialValue = 0; }

    SECTION("Initial value 4")
    {
        constexpr unsigned int cInitialValue = 4;
        initialValue = cInitialValue;
    }

    SECTION("Initial value 15")
    {
        constexpr unsigned int cInitialValue = 15;
        initialValue = cInitialValue;
    }

    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, initialValue);
    REQUIRE(error == OsalError::eOk);

    for (unsigned int i = 0; i < initialValue; ++i) {
        error = osalSemaphoreTryWait(&semaphore);
        REQUIRE(error == OsalError::eOk);
    }

    error = osalSemaphoreTryWait(&semaphore);
    REQUIRE(error == OsalError::eLocked);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("Multiple tryWait called from ISR", "[unit][c][semaphore]")
{
    unsigned int initialValue{};

    SECTION("Initial value 0") { initialValue = 0; }

    SECTION("Initial value 4")
    {
        constexpr unsigned int cInitialValue = 4;
        initialValue = cInitialValue;
    }

    SECTION("Initial value 15")
    {
        constexpr unsigned int cInitialValue = 15;
        initialValue = cInitialValue;
    }

    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, initialValue);
    REQUIRE(error == OsalError::eOk);

    for (unsigned int i = 0; i < initialValue; ++i) {
        error = osalSemaphoreTryWaitIsr(&semaphore);
        REQUIRE(error == OsalError::eOk);
    }

    error = osalSemaphoreTryWaitIsr(&semaphore);
    REQUIRE(error == OsalError::eLocked);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("TimedWait called from second thread, timeout", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        constexpr std::uint32_t cTimeoutMs = 100;
        auto error = osalSemaphoreTimedWait(&semaphore, cTimeoutMs);
        if (error != OsalError::eTimeout)
            REQUIRE(error == OsalError::eTimeout);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);
    };

    osal::Thread thread(func);
    thread.join();

    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}

TEST_CASE("TimedWait called from second thread, success", "[unit][c][semaphore]")
{
    OsalSemaphore semaphore{};
    auto error = osalSemaphoreCreate(&semaphore, 1);
    REQUIRE(error == OsalError::eOk);

    error = osalSemaphoreWait(&semaphore);
    REQUIRE(error == OsalError::eOk);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        constexpr std::uint32_t cTimeoutMs = 100;
        if (auto error = osalSemaphoreTimedWait(&semaphore, cTimeoutMs))
            REQUIRE(!error);

        auto end = osal::timestamp();
        if ((end - start) > 100ms)
            REQUIRE((end - start) <= 100ms);

        if (auto error = osalSemaphoreSignal(&semaphore))
            REQUIRE(!error);
    };

    osal::Thread thread(func);
    osal::sleep(50ms);

    error = osalSemaphoreSignal(&semaphore);
    REQUIRE(error == OsalError::eOk);

    thread.join();

    error = osalSemaphoreDestroy(&semaphore);
    REQUIRE(error == OsalError::eOk);
}
