/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2020-2022, Kuba Sejdak <kuba.sejdak@gmail.com>
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

#include "osal/Mutex.h"

#include "osal/common/logger.hpp"
#include "osal/timestamp.h"

#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <ctime>

OsalError osalMutexCreate(OsalMutex* mutex, OsalMutexType type)
{
    if (mutex == nullptr) {
        MutexLogger::error("Failed to create mutex: mutex=nullptr");
        return OsalError::eInvalidArgument;
    }

    mutex->initialized = false;

    auto mutexType = (type == OsalMutexType::eRecursive) ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_NORMAL;

    pthread_mutexattr_t attr{};
    pthread_mutexattr_init(&attr);
    [[maybe_unused]] auto result = pthread_mutexattr_settype(&attr, mutexType);
    assert(result == 0);

    pthread_mutex_t handle{};
    result = pthread_mutex_init(&handle, &attr);
    assert(result == 0);

    pthread_mutexattr_destroy(&attr);
    assert(result == 0);

    mutex->impl.handle = handle;
    mutex->type = type;
    mutex->initialized = true;

    MutexLogger::trace("Created mutex: type={}", type);
    return OsalError::eOk;
}

OsalError osalMutexDestroy(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to destroy mutex: invalid argument");
        return OsalError::eInvalidArgument;
    }

    [[maybe_unused]] auto result = pthread_mutex_destroy(&mutex->impl.handle);
    assert(result == 0);

    std::memset(mutex, 0, sizeof(OsalMutex));
    MutexLogger::trace("Destroyed mutex");
    return OsalError::eOk;
}

OsalError osalMutexLock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to lock mutex: invalid argument");
        return OsalError::eInvalidArgument;
    }

    [[maybe_unused]] auto result = pthread_mutex_lock(&mutex->impl.handle);
    assert(result == 0);

    MutexLogger::trace("Locked mutex");
    return OsalError::eOk;
}

OsalError osalMutexTryLock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to tryLock mutex: invalid argument");
        return OsalError::eInvalidArgument;
    }

    auto result = pthread_mutex_trylock(&mutex->impl.handle);
    switch (result) {
        case EAGAIN: [[fallthrough]];
        case EBUSY:
            MutexLogger::debug("Failed to lock mutex: mutex locked by another client");
            return OsalError::eLocked;
        default: break;
    }

    assert(result == 0);

    MutexLogger::trace("Locked mutex");
    return OsalError::eOk;
}

OsalError osalMutexTryLockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to tryLock mutex in ISR: invalid argument");
        return OsalError::eInvalidArgument;
    }

    if (mutex->type == OsalMutexType::eRecursive) {
        MutexLogger::error("Failed to tryLock mutex in ISR: mutex is recursive");
        return OsalError::eInvalidArgument;
    }

    return osalMutexTryLock(mutex);
}

OsalError osalMutexTimedLock(OsalMutex* mutex, uint32_t timeoutMs)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to timedLock mutex: invalid argument");
        return OsalError::eInvalidArgument;
    }

    auto toTimespec = [](const auto& timePoint) {
        auto secs = std::chrono::time_point_cast<std::chrono::seconds>(timePoint);
        auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(timePoint)
                - std::chrono::time_point_cast<std::chrono::nanoseconds>(secs);

        return timespec{int(secs.time_since_epoch().count()), int(ns.count())};
    };

    auto ts = toTimespec(std::chrono::system_clock::now() + std::chrono::milliseconds{timeoutMs});
    auto result = pthread_mutex_timedlock(&mutex->impl.handle, &ts);
    if (result == ETIMEDOUT) {
        MutexLogger::debug("Failed to timedLock mutex: timeout, timeoutMs={}", timeoutMs);
        return OsalError::eTimeout;
    }

    assert(result == 0);
    MutexLogger::trace("Locked mutex");
    return OsalError::eOk;
}

OsalError osalMutexUnlock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized) {
        MutexLogger::error("Failed to unlock mutex: invalid argument");
        return OsalError::eInvalidArgument;
    }

    [[maybe_unused]] auto result = pthread_mutex_unlock(&mutex->impl.handle);
    assert(result == 0);
    MutexLogger::trace("Unlocked mutex");
    return OsalError::eOk;
}

OsalError osalMutexUnlockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || mutex->type == OsalMutexType::eRecursive) {
        MutexLogger::error("Failed to unlock mutex in ISR: invalid argument");
        return OsalError::eInvalidArgument;
    }

    return osalMutexUnlock(mutex);
}
