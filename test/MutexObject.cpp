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

#include <osal/Mutex.hpp>
#include <osal/Thread.hpp>
#include <osal/sleep.hpp>
#include <osal/timestamp.hpp>

#include <catch2/catch.hpp>

#include <utility>

TEST_CASE("Mutex creation and destruction in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Recursive mutex") { type = OsalMutexType::eRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex;
    auto error = mutex.lock();
    REQUIRE(!error);

    error = mutex.unlock();
    REQUIRE(!error);

    osal::Mutex mutex2(type);
    error = mutex2.lock();
    REQUIRE(!error);

    error = mutex2.unlock();
    REQUIRE(!error);
}

TEST_CASE("Moving mutex around in C++", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;
    auto error = mutex.lock();
    REQUIRE(!error);

    osal::Mutex mutex2(std::move(mutex));

    error = mutex.unlock(); // NOLINT
    REQUIRE(error == OsalError::eInvalidArgument);

    error = mutex2.unlock();
    REQUIRE(!error);
}

TEST_CASE("Lock called from two threads in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Recursive mutex") { type = OsalMutexType::eRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex(type);

    auto error = mutex.lock();
    REQUIRE(!error);

    auto func = [&mutex] {
        auto start = osal::timestamp();

        if (auto error = mutex.lock())
            REQUIRE(!error);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        if (auto error = mutex.unlock())
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = mutex.unlock();
    REQUIRE(!error);
}

TEST_CASE("TryLock called from second threads in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Recursive mutex") { type = OsalMutexType::eRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex(type);

    auto error = mutex.lock();
    REQUIRE(!error);

    auto func = [&mutex] {
        auto start = osal::timestamp();

        while (mutex.tryLock() != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        if (auto error = mutex.unlock())
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = mutex.unlock();
    REQUIRE(!error);

    thread.join();
}

TEST_CASE("TryLock and unlock called from ISR in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex(type);

    auto error = mutex.tryLockIsr();
    REQUIRE(!error);

    auto func = [&mutex] {
        auto start = osal::timestamp();

        while (mutex.tryLockIsr() != OsalError::eOk)
            osal::sleep(10ms);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);

        if (auto error = mutex.unlockIsr())
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(120ms);
    error = mutex.unlock();
    REQUIRE(!error);

    thread.join();
}

TEST_CASE("Recursive tryLock called from ISR in C++", "[unit][cpp][mutex]")
{
    osal::Mutex mutex(OsalMutexType::eRecursive);

    auto error = mutex.tryLockIsr();
    REQUIRE(error == OsalError::eInvalidArgument);
}

TEST_CASE("TimedLock called from second thread, timeout in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Recursive mutex") { type = OsalMutexType::eRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex(type);

    auto error = mutex.lock();
    REQUIRE(!error);

    auto func = [&mutex] {
        auto start = osal::timestamp();

        auto error = mutex.timedLock(100ms);
        if (error != OsalError::eTimeout)
            REQUIRE(error == OsalError::eTimeout);

        auto end = osal::timestamp();
        if ((end - start) < 100ms)
            REQUIRE((end - start) >= 100ms);
    };

    osal::Thread thread(func);
    thread.join();

    error = mutex.unlock();
    REQUIRE(!error);
}

TEST_CASE("TimedLock called from second thread, success in C++", "[unit][cpp][mutex]")
{
    OsalMutexType type{};

    SECTION("Non recursive mutex") { type = OsalMutexType::eNonRecursive; }

    SECTION("Recursive mutex") { type = OsalMutexType::eRecursive; }

    SECTION("Default mutex") { type = cOsalMutexDefaultType; }

    osal::Mutex mutex(type);

    auto error = mutex.lock();
    REQUIRE(!error);

    auto func = [&mutex] {
        auto start = osal::timestamp();

        if (auto error = mutex.timedLock(100ms))
            REQUIRE(!error);

        auto end = osal::timestamp();
        if ((end - start) > 100ms)
            REQUIRE((end - start) <= 100ms);

        if (auto error = mutex.unlock())
            REQUIRE(!error);
    };

    osal::Thread thread(func);

    osal::sleep(50ms);
    error = mutex.unlock();
    REQUIRE(!error);
}

TEST_CASE("Timeout used with mutexes", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;
    auto error = mutex.lock();
    REQUIRE(!error);

    osal::Timeout timeout = 100ms;
    error = mutex.timedLock(timeout);
    REQUIRE(error == OsalError::eTimeout);
    REQUIRE(timeout.isExpired());

    error = mutex.unlock();
    REQUIRE(!error);
}
