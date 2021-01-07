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

#include "osal/Semaphore.h"

#include "osal/timestamp.h"

#include <semaphore.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>

OsalError osalSemaphoreCreate(OsalSemaphore* semaphore, unsigned int initialValue)
{
    if (semaphore == nullptr)
        return OsalError::eInvalidArgument;

    sem_t handle{};
    [[maybe_unused]] auto result = sem_init(&handle, 0, initialValue);
    assert(result == 0);

    semaphore->impl.handle = handle;
    semaphore->initialized = true;
    return OsalError::eOk;
}

OsalError osalSemaphoreDestroy(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    [[maybe_unused]] auto result = sem_destroy(&semaphore->impl.handle);
    assert(result == 0);

    std::memset(semaphore, 0, sizeof(OsalSemaphore));
    return OsalError::eOk;
}

OsalError osalSemaphoreWait(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    [[maybe_unused]] auto result = sem_wait(&semaphore->impl.handle);
    assert(result == 0);
    return OsalError::eOk;
}

OsalError osalSemaphoreTryWait(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    auto result = sem_trywait(&semaphore->impl.handle);
    if (result == -1)
        return OsalError::eLocked;

    assert(result == 0);
    return OsalError::eOk;
}

OsalError osalSemaphoreTryWaitIsr(OsalSemaphore* semaphore)
{
    return osalSemaphoreTryWait(semaphore);
}

OsalError osalSemaphoreTimedWait(OsalSemaphore* semaphore, uint32_t timeoutMs)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    timespec ts{};
    auto result = clock_gettime(CLOCK_REALTIME, &ts);
    assert(result == 0);

    ts.tv_nsec += osalMsToNs(timeoutMs);
    auto secs = osalNsToSec(ts.tv_nsec);
    ts.tv_sec += secs;
    ts.tv_nsec -= osalSecToNs(secs);

    result = sem_timedwait(&semaphore->impl.handle, &ts);
    if ((result == -1) && (errno == ETIMEDOUT))
        return OsalError::eTimeout;

    assert(result == 0);
    return OsalError::eOk;
}

OsalError osalSemaphoreSignal(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    [[maybe_unused]] auto result = sem_post(&semaphore->impl.handle);
    assert(result == 0);
    return OsalError::eOk;
}

OsalError osalSemaphoreSignalIsr(OsalSemaphore* semaphore)
{
    return osalSemaphoreSignal(semaphore);
}
