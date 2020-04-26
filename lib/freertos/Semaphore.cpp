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

#include "osal/Semaphore.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstring>
#include <limits>

OsalError osalSemaphoreCreate(OsalSemaphore* semaphore, unsigned int initValue)
{
    if (semaphore == nullptr)
        return OsalError::eInvalidArgument;

    semaphore->initialized = false;

    SemaphoreHandle_t handle{};
#if configSUPPORT_STATIC_ALLOCATION
    handle = xSemaphoreCreateCountingStatic(std::numeric_limits<BaseType_t>::max(), initValue, &semaphore->impl.buffer);
#elif configSUPPORT_DYNAMIC_ALLOCATION
    handle = xSemaphoreCreateCounting(std::numeric_limits<BaseType_t>::max(), initValue);
#endif

    if (handle == nullptr)
        return OsalError::eOsError;

    semaphore->impl.handle = handle;
    semaphore->initialized = true;
    return OsalError::eOk;
}

OsalError osalSemaphoreDestroy(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    vSemaphoreDelete(semaphore->impl.handle);
    std::memset(semaphore, 0, sizeof(OsalSemaphore));
    return OsalError::eOk;
}

OsalError osalSemaphoreWait(OsalSemaphore* semaphore)
{
    auto error = osalSemaphoreTimedWait(semaphore, portMAX_DELAY);
    configASSERT(error == OsalError::eOk);
    return error;
}

OsalError osalSemaphoreTryWait(OsalSemaphore* semaphore)
{
    auto error = osalSemaphoreTimedWait(semaphore, 0);
    return (error == OsalError::eTimeout) ? OsalError::eLocked : error;
}

OsalError osalSemaphoreTryWaitIsr(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    if (xSemaphoreTakeFromISR(semaphore->impl.handle, nullptr) == pdFALSE)
        return OsalError::eLocked;

    return OsalError::eOk;
}

OsalError osalSemaphoreTimedWait(OsalSemaphore* semaphore, uint32_t timeoutMs)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    TickType_t tickTimeout = (timeoutMs == portMAX_DELAY) ? portMAX_DELAY : (timeoutMs / portTICK_PERIOD_MS);
    if (xSemaphoreTake(semaphore->impl.handle, tickTimeout) == pdFALSE)
        return OsalError::eTimeout;

    return OsalError::eOk;
}

OsalError osalSemaphoreSignal(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    if (xSemaphoreGive(semaphore->impl.handle) == pdFALSE)
        return OsalError::eOsError;

    return OsalError::eOk;
}

OsalError osalSemaphoreSignalIsr(OsalSemaphore* semaphore)
{
    if (semaphore == nullptr || !semaphore->initialized)
        return OsalError::eInvalidArgument;

    if (xSemaphoreGiveFromISR(semaphore->impl.handle, nullptr) == pdFALSE)
        return OsalError::eOsError;

    return OsalError::eOk;
}
