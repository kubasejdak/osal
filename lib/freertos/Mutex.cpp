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

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstring>

OsalError osalMutexCreate(OsalMutex* mutex, OsalMutexType type)
{
    if (mutex == nullptr)
        return OsalError::eInvalidArgument;

    mutex->initialized = false;

    SemaphoreHandle_t handle{};
#if configSUPPORT_STATIC_ALLOCATION
    switch (type) {
    #if configUSE_RECURSIVE_MUTEXES
        case OsalMutexType::eRecursive: handle = xSemaphoreCreateRecursiveMutexStatic(&mutex->impl.buffer); break;
    #endif
        case OsalMutexType::eNonRecursive: handle = xSemaphoreCreateMutexStatic(&mutex->impl.buffer); break;
        default: return OsalError::eInvalidArgument;
    }
#elif configSUPPORT_DYNAMIC_ALLOCATION
    switch (type) {
    #if configUSE_RECURSIVE_MUTEXES
        case OsalMutexType::eRecursive: handle = xSemaphoreCreateRecursiveMutex(&mutex->impl.buffer); break;
    #endif
        case OsalMutexType::eNonRecursive: handle = xSemaphoreCreateMutex(); break;
        default: return OsalError::eInvalidArgument;
    }
#endif

    if (handle == nullptr)
        return OsalError::eOsError;

    mutex->impl.handle = handle;
    mutex->initialized = true;
    mutex->type = type;
    return OsalError::eOk;
}

OsalError osalMutexDestroy(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    vSemaphoreDelete(mutex->impl.handle);
    std::memset(mutex, 0, sizeof(OsalMutex));
    return OsalError::eOk;
}

OsalError osalMutexLock(OsalMutex* mutex)
{
    auto error = osalMutexTimedLock(mutex, portMAX_DELAY);
    configASSERT(error == OsalError::eOk);
    return error;
}

OsalError osalMutexTryLock(OsalMutex* mutex)
{
    auto error = osalMutexTimedLock(mutex, 0);
    return (error == OsalError::eTimeout) ? OsalError::eLocked : error;
}

OsalError osalMutexTryLockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    if (mutex->type == OsalMutexType::eRecursive)
        return OsalError::eInvalidArgument;

    if (xSemaphoreTakeFromISR(mutex->impl.handle, nullptr) == pdFALSE)
        return OsalError::eLocked;

    return OsalError::eOk;
}

OsalError osalMutexTimedLock(OsalMutex* mutex, uint32_t timeoutMs)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    BaseType_t result{};
    TickType_t tickTimeout = (timeoutMs == portMAX_DELAY) ? portMAX_DELAY : (timeoutMs / portTICK_PERIOD_MS);

    if (mutex->type == OsalMutexType::eRecursive) {
#if configUSE_RECURSIVE_MUTEXES
        result = xSemaphoreTakeRecursive(mutex->impl.handle, tickTimeout);
#endif
    }
    else
        result = xSemaphoreTake(mutex->impl.handle, tickTimeout);

    if (result == pdFALSE)
        return OsalError::eTimeout;

    return OsalError::eOk;
}

OsalError osalMutexUnlock(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    BaseType_t result{};

    if (mutex->type == OsalMutexType::eRecursive) {
#if configUSE_RECURSIVE_MUTEXES
        result = xSemaphoreGiveRecursive(mutex->impl.handle);
#endif
    }
    else
        result = xSemaphoreGive(mutex->impl.handle);

    if (result == pdFALSE)
        return OsalError::eOsError;

    return OsalError::eOk;
}

OsalError osalMutexUnlockIsr(OsalMutex* mutex)
{
    if (mutex == nullptr || !mutex->initialized)
        return OsalError::eInvalidArgument;

    if (mutex->type == OsalMutexType::eRecursive)
        return OsalError::eInvalidArgument;

    if (xSemaphoreGiveFromISR(mutex->impl.handle, nullptr) == pdFALSE)
        return OsalError::eOsError;

    return OsalError::eOk;
}
