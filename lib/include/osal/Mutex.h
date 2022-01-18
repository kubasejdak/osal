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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Error.h"
#include "internal/MutexImpl.h"

#include <stdint.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

/// Represents possible types of the OSAL mutex. These types define how mutex will react to multiple recursive
/// locks made by the same thread.
enum OsalMutexType {
    eRecursive,
    eNonRecursive
};

/// Represents OSAL mutex handle.
/// @note Size of this structure depends on the concrete implementation. In particular, MutexImpl
///       contains objects from the target platform. Thus depending on its size is not recommended.
struct OsalMutex {
    MutexImpl impl;
    OsalMutexType type;
    bool initialized;
};

/// Helper constant with default mutex type.
static const OsalMutexType cOsalMutexDefaultType = OsalMutexType::eNonRecursive;

/// Creates new mutex with the given type.
/// @param mutex            Mutex handle to be initialized.
/// @param type             Type of the mutex to be created.
/// @return Error code of the operation.
/// @note Created mutex is in unlocked state.
OsalError osalMutexCreate(OsalMutex* mutex, OsalMutexType type);

/// Destroys mutex represented by the given handle.
/// @param mutex            Mutex handle to be destroyed.
/// @return Error code of the operation.
OsalError osalMutexDestroy(OsalMutex* mutex);

/// Locks the given mutex. If it is currently locked any thread, then the calling thread will block until mutex
/// is released. If mutex is recursive, then it can be locked multiple times by the same thread.
/// @param mutex            Mutex to be locked.
/// @return Error code of the operation.
/// @note If mutex is non-recursive, then calling this function twice by the same thread will result in a deadlock.
OsalError osalMutexLock(OsalMutex* mutex);

/// Locks the given mutex. If it is currently locked any thread, then it returns immediately with a proper error.
/// @param mutex            Mutex to be locked.
/// @return Error code of the operation.
/// @note If mutex is non-recursive, then calling this function twice by the same thread will not result in a deadlock.
OsalError osalMutexTryLock(OsalMutex* mutex);

/// Locks the given mutex. If it is currently locked any thread, then it returns immediately with a proper error.
/// @param mutex            Mutex to be locked.
/// @return Error code of the operation.
/// @note This function will never block and is supposed to be called from ISR.
OsalError osalMutexTryLockIsr(OsalMutex* mutex);

/// Locks the given mutex. If it is currently locked any thread, then the calling thread will block until mutex
/// is released or the specified time elapses. If mutex is recursive, then it can be locked multiple times by the
/// same thread.
/// @param mutex            Mutex to be locked.
/// @param timeoutMs        Maximal time in ms to wait for the operation.
/// @return Error code of the operation.
OsalError osalMutexTimedLock(OsalMutex* mutex, uint32_t timeoutMs);

/// Unlocks the given mutex.
/// @param mutex            Mutex to be unlocked.
/// @return Error code of the operation.
/// @note Unlocking mutex that was already locked by another thread invokes undefined behavior.
OsalError osalMutexUnlock(OsalMutex* mutex);

/// Unlocks the given mutex.
/// @param mutex            Mutex to be unlocked.
/// @return Error code of the operation.
/// @note This function will never block and is supposed to be called from ISR.
/// @note Unlocking mutex that was already locked by another thread invokes undefined behavior.
OsalError osalMutexUnlockIsr(OsalMutex* mutex);

#ifdef __cplusplus
}
#endif
