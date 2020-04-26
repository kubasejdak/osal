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

#include <osal/Semaphore.hpp>
#include <osal/Thread.hpp>
#include <osal/sleep.hpp>
#include <osal/timestamp.hpp>

#include <catch2/catch.hpp>

#include <utility>

TEST_CASE("Semaphore creation and destruction in C++", "[unit][cpp][semaphore]")
{
    unsigned int initialValue{};

    SECTION("Initial value 0") { initialValue = 0; }

    SECTION("Initial value 1") { initialValue = 1; }

    SECTION("Initial value 15")
    {
        constexpr unsigned int cInitialValue = 15;
        initialValue = cInitialValue;
    }

    osal::Semaphore semaphore(initialValue);

    if (initialValue != 0) {
        auto error = semaphore.wait();
        REQUIRE(!error);
    }

    auto error = semaphore.signal();
    REQUIRE(!error);
}

TEST_CASE("Moving semaphore around in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);
    auto error = semaphore.wait();
    REQUIRE(!error);

    osal::Semaphore semaphore2(std::move(semaphore));

    error = semaphore.signal(); // NOLINT
    REQUIRE(error == OsalError::eInvalidArgument);

    error = semaphore2.signal();
    REQUIRE(!error);
}

TEST_CASE("Wait and signal from one thread in C++, start with 1", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.wait();
    REQUIRE(!error);

    error = semaphore.signal();
    REQUIRE(!error);
}

TEST_CASE("Wait and signal from one thread in C++, start with 0", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(0);

    auto error = semaphore.signal();
    REQUIRE(!error);

    error = semaphore.wait();
    REQUIRE(!error);
}

TEST_CASE("Combination of wait and signal calls from one thread in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(4);

    // 3.
    auto error = semaphore.wait();
    REQUIRE(!error);

    // 2.
    error = semaphore.wait();
    REQUIRE(!error);

    // 1.
    error = semaphore.wait();
    REQUIRE(!error);

    // 2.
    error = semaphore.signal();
    REQUIRE(!error);

    // 1.
    error = semaphore.wait();
    REQUIRE(!error);

    // 2.
    error = semaphore.signal();
    REQUIRE(!error);

    // 3.
    error = semaphore.signal();
    REQUIRE(!error);

    int count{};
    while (semaphore.tryWait() == OsalError::eOk)
        ++count;

    REQUIRE(count == 3);
}

TEST_CASE("Increment semaphore to value bigger than the initial one in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(4);
    int firstCount{};
    while (semaphore.tryWait() == OsalError::eOk)
        ++firstCount;

    constexpr int cFirstExpectedCount = 4;
    REQUIRE(firstCount == cFirstExpectedCount);

    constexpr int cSecondExpectedCount = 16;
    for (int i = 0; i < cSecondExpectedCount; ++i) {
        auto error = semaphore.signal();
        REQUIRE(!error);
    }

    int secondCount{};
    while (semaphore.tryWait() == OsalError::eOk)
        ++secondCount;

    REQUIRE(secondCount == cSecondExpectedCount);
}

TEST_CASE("Wait called from two threads in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.wait();
    REQUIRE(!error);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        auto error = semaphore.wait();
        if (error != OsalError::eOk)
            REQUIRE(!error);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        error = semaphore.signal();
        if (error != OsalError::eOk)
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = semaphore.signal();
    REQUIRE(!error);
}

TEST_CASE("TryWait called from second thread in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.wait();
    REQUIRE(!error);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        while (semaphore.tryWait() != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        auto error = semaphore.signal();
        if (error != OsalError::eOk)
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = semaphore.signal();
    REQUIRE(!error);
}

TEST_CASE("TryWait and signal called from ISR in C++", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.tryWaitIsr();
    REQUIRE(!error);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        while (semaphore.tryWaitIsr() != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        auto error = semaphore.signal();
        if (error != OsalError::eOk)
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = semaphore.signalIsr();
    REQUIRE(!error);
}

TEST_CASE("Multiple wait called one thread in C++", "[unit][cpp][semaphore]")
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

    osal::Semaphore semaphore(initialValue);

    for (unsigned int i = 0; i < initialValue; ++i) {
        auto error = semaphore.wait();
        REQUIRE(!error);
    }

    auto error = semaphore.tryWait();
    REQUIRE(error == OsalError::eLocked);
}

TEST_CASE("Multiple tryWait called one thread in C++", "[unit][cpp][semaphore]")
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

    osal::Semaphore semaphore(initialValue);

    for (unsigned int i = 0; i < initialValue; ++i) {
        auto error = semaphore.tryWait();
        REQUIRE(!error);
    }

    auto error = semaphore.tryWait();
    REQUIRE(error == OsalError::eLocked);
}

TEST_CASE("Multiple tryWait called from ISR in C++", "[unit][cpp][semaphore]")
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

    osal::Semaphore semaphore(initialValue);

    for (unsigned int i = 0; i < initialValue; ++i) {
        auto error = semaphore.tryWaitIsr();
        REQUIRE(!error);
    }

    auto error = semaphore.tryWaitIsr();
    REQUIRE(error == OsalError::eLocked);
}

TEST_CASE("TimedWait called from second thread in C++, timeout", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.wait();
    REQUIRE(!error);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        auto error = semaphore.timedWait(100ms);
        if (error != OsalError::eTimeout)
            REQUIRE(error == OsalError::eTimeout);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);
    };

    osal::Thread thread(func);
    thread.join();

    error = semaphore.signal();
    REQUIRE(!error);
}

TEST_CASE("TimedWait called from second thread in C++, success", "[unit][cpp][semaphore]")
{
    osal::Semaphore semaphore(1);

    auto error = semaphore.wait();
    REQUIRE(!error);

    auto func = [&semaphore] {
        auto start = osal::timestamp();

        if (auto error = semaphore.timedWait(100ms))
            REQUIRE(!error);

        auto end = osal::timestamp();
        if ((end - start) > 100ms)
            REQUIRE((end - start) <= 100ms);

        if (auto error = semaphore.signal())
            REQUIRE(!error);
    };

    osal::Thread thread(func);
    osal::sleep(50ms);

    error = semaphore.signal();
    REQUIRE(!error);
}
