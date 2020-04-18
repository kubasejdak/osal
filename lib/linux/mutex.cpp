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

#include "osal/mutex.h"

#include "osal/timestamp.h"

#include <cassert>
#include <cerrno>
#include <cstring>

OsalError osalMutexCreate(OsalMutex* mutex, OsalMutexType type)
{
    if (mutex == nullptr)
        return OsalError::eInvalidArgument;

    mutex->initialized = false;

    int mutexType;
    switch (type) {
        case OsalMutexType::eRecursive: mutexType = PTHREAD_MUTEX_RECURSIVE; break;
        case OsalMutexType::eNonRecursive: mutexType = PTHREAD_MUTEX_NORMAL; break;
        default: return OsalError::eInvalidArgument;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    [[maybe_unused]] auto result = pthread_mutexattr_settype(&attr, mutexType);
    assert(result == 0);

    pthread_mutex_t handle;
    result = pthread_mutex_init(&handle, &attr);
    assert(result == 0);

    pthread_mutexattr_destroy(&attr);
    assert(result == 0);

    mutex->impl.handle = handle;
    mutex->type = type;
    mutex->initialized = true;
    return OsalError::eOk;
}

OsalError osalMutexDestroy(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    [[maybe_unused]] auto result = pthread_mutex_destroy(&mutex->impl.handle);
    assert(result == 0);

    std::memset(mutex, 0, sizeof(OsalMutex));
    return OsalError::eOk;
}

OsalError osalMutexLock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    auto result = pthread_mutex_lock(&mutex->impl.handle);
    switch (result) {
        case 0: return OsalError::eOk;
        case EINVAL: return OsalError::eInvalidArgument;
        case EAGAIN: [[fallthrough]];
        case EBUSY: return OsalError::eLocked;
        default: break;
    }

    return OsalError::eOsError;
}

OsalError osalMutexTryLock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    auto result = pthread_mutex_trylock(&mutex->impl.handle);
    switch (result) {
        case 0: return OsalError::eOk;
        case EINVAL: return OsalError::eInvalidArgument;
        case EAGAIN: [[fallthrough]];
        case EBUSY: return OsalError::eLocked;
        default: break;
    }

    return OsalError::eOsError;
}

OsalError osalMutexTryLockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || mutex->type == OsalMutexType::eRecursive)
        return OsalError::eInvalidArgument;

    return osalMutexTryLock(mutex);
}

OsalError osalMutexTimedLock(OsalMutex* mutex, uint32_t timeoutMs)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    auto timeoutSec = osalMsToSec(timeoutMs);
    auto timeoutNs = osalMsToNs(timeoutMs - osalSecToMs(timeoutSec));

    timespec ts{};
    auto result = clock_gettime(CLOCK_REALTIME, &ts);
    assert(result == 0);

    ts.tv_sec += timeoutSec;
    ts.tv_nsec += timeoutNs;

    result = pthread_mutex_timedlock(&mutex->impl.handle, &ts);
    switch (result) {
        case 0: return OsalError::eOk;
        case EINVAL: return OsalError::eInvalidArgument;
        case EAGAIN: [[fallthrough]];
        case EBUSY: return OsalError::eLocked;
        case ETIMEDOUT: return OsalError::eTimeout;
        default: break;
    }

    return OsalError::eOsError;
}

OsalError osalMutexUnlock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    auto result = pthread_mutex_unlock(&mutex->impl.handle);
    switch (result) {
        case 0: return OsalError::eOk;
        case EINVAL: return OsalError::eInvalidArgument;
        case EAGAIN: [[fallthrough]];
        case EBUSY: return OsalError::eLocked;
        default: break;
    }

    return OsalError::eOsError;
}

OsalError osalMutexUnlockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || mutex->type == OsalMutexType::eRecursive)
        return OsalError::eInvalidArgument;

    return osalMutexUnlock(mutex);
}
