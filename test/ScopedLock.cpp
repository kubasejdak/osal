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

#include <osal/Mutex.hpp>
#include <osal/ScopedLock.hpp>
#include <osal/Thread.hpp>
#include <osal/sleep.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Create and destroy lock", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;
    osal::ScopedLock lock(mutex);
    bool locked = lock;
    bool acquired = lock.isAcquired();

    REQUIRE(locked);
    REQUIRE(acquired);

    auto error = mutex.tryLock();
    REQUIRE(error == OsalError::eLocked);
}

TEST_CASE("Create lock with timeout", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;

    {
        osal::ScopedLock lock(mutex, 100ms);
        bool locked = lock;
        REQUIRE(locked);
    }

    auto error = mutex.lock();
    REQUIRE(!error);

    {
        osal::ScopedLock lock(mutex, 100ms);
        bool locked = lock;
        REQUIRE(!locked);
    }
}

TEST_CASE("Unlock ScopedLock when destroyed", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;

    {
        osal::ScopedLock lock(mutex);
        bool locked = lock;
        REQUIRE(locked);

        auto error = mutex.tryLock();
        REQUIRE(error == OsalError::eLocked);
    }

    auto error = mutex.tryLock();
    REQUIRE(!error);

    error = mutex.unlock();
    REQUIRE(!error);
}

TEST_CASE("Non-recursive success waitLock() thread with ScopedLock in C++", "[unit][cpp][mutex]")
{
    osal::Mutex mutex;
    std::error_code threadError;

    auto func = [&mutex, &threadError] {
        osal::ScopedLock lock(mutex, 500ms);
        threadError = lock ? OsalError::eOk : OsalError::eTimeout;
    };

    osal::Thread<decltype(func)> thread;

    {
        osal::ScopedLock lock(mutex);
        bool locked = lock;
        REQUIRE(locked);

        thread.start(func);
        osal::sleep(100ms);
    }

    auto error = thread.join();
    REQUIRE(!error);
    REQUIRE(!threadError);
}
