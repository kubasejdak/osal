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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Error.h"
#include "internal/SemaphoreImpl.h"

#include <stdint.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

/// Represents OSAL semaphore handle.
/// @note Size of this structure depends on the concrete implementation. In particular, SemaphoreImpl
///       contains objects from the target platform. Thus depending on its size is not recommended.
struct OsalSemaphore {
    SemaphoreImpl impl;
    bool initialized;
};

/// Creates new semaphore with the given initial value.
/// @param semaphore        Semaphore handle to be initialized.
/// @param initialValue     Initial value of the semaphore to be created.
/// @return Error code of the operation.
OsalError osalSemaphoreCreate(OsalSemaphore* semaphore, unsigned int initialValue);

/// Destroys semaphore represented by the given handle.
/// @param semaphore        Semaphore handle to be destroyed.
/// @return Error code of the operation.
OsalError osalSemaphoreDestroy(OsalSemaphore* semaphore);

/// Decrements value of the given semaphore. If its value is currently 0, then the calling thread will block until
/// semaphore is positive again.
/// @param semaphore        Semaphore to be decremented.
/// @return Error code of the operation.
OsalError osalSemaphoreWait(OsalSemaphore* semaphore);

/// Decrements value of the given semaphore. If its value is currently 0, then it returns immediately
/// with a proper error.
/// @param semaphore        Semaphore to be decremented.
/// @return Error code of the operation.
OsalError osalSemaphoreTryWait(OsalSemaphore* semaphore);

/// Decrements value of the given semaphore. If its value is currently 0, then it returns immediately
/// with a proper error.
/// @param semaphore        Semaphore to be decremented.
/// @return Error code of the operation.
/// @note This function will never block and is supposed to be called from ISR.
OsalError osalSemaphoreTryWaitIsr(OsalSemaphore* semaphore);

/// Decrements value of the given semaphore. If its value is currently 0, then the calling thread will block until
/// semaphore is positive again or the specified time elapses.
/// @param semaphore        Semaphore to be decremented.
/// @param timeoutMs        Maximal time in ms to wait for the operation.
/// @return Error code of the operation.
OsalError osalSemaphoreTimedWait(OsalSemaphore* semaphore, uint32_t timeoutMs);

/// Increments value of the given semaphore.
/// @param semaphore        Semaphore to be incremented.
/// @return Error code of the operation.
/// @note There is no upper bound of the semaphore value.
OsalError osalSemaphoreSignal(OsalSemaphore* semaphore);

/// Increments value of the given semaphore.
/// @param semaphore        Semaphore to be incremented.
/// @return Error code of the operation.
/// @note This function will never block and is supposed to be called from ISR.
/// @note There is no upper bound of the semaphore value.
OsalError osalSemaphoreSignalIsr(OsalSemaphore* semaphore);

#ifdef __cplusplus
}
#endif
