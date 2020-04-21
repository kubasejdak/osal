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

#pragma once

#include "osal/Error.hpp"
#include "osal/Mutex.h"
#include "osal/Timeout.hpp"

#include <chrono>
#include <string>
#include <system_error>

namespace osal {

/// @class Mutex
/// Represents OSAL mutex handle.
class Mutex {
public:
    /// Constructor. Creates new mutex with the given type.
    /// @param type         Mutex type.
    explicit Mutex(OsalMutexType type = cOsalMutexDefaultType);

    /// Copy constructor.
    /// @note This constructor is deleted, because Mutex is not meant to be copied.
    Mutex(const Mutex&) = delete;

    /// move constructor.
    /// @param other        Object to be moved from.
    Mutex(Mutex&& other) noexcept;

    /// Destructor.
    ~Mutex();

    /// Copy assignment operator.
    /// @note This operator is deleted, because Mutex is not meant to be copied.
    Mutex& operator=(const Mutex&) = delete;

    /// Move assignment operator.
    /// @note This operator is deleted, because Mutex is not meant to be move assigned.
    Mutex& operator=(Mutex&&) = delete;

    /// Locks the given mutex. If it is currently locked any thread, then the calling thread will block until mutex
    /// is released. If mutex is recursive, then it can be locked multiple times by the same thread.
    /// @return Error code of the operation.
    /// @note If mutex is non-recursive, then calling this function twice by the same thread will result in a deadlock.
    std::error_code lock();

    /// Locks the given mutex. If it is currently locked any thread, then it returns immediately with a proper error.
    /// @return Error code of the operation.
    /// @note If mutex is non-recursive, then calling this function twice by the same thread will not result in a
    /// deadlock.
    std::error_code tryLock();

    /// Locks the given mutex. If it is currently locked any thread, then it returns immediately with a proper error.
    /// @return Error code of the operation.
    /// @note This function will never block and is supposed to be called from ISR.
    std::error_code tryLockIsr();

    /// Locks the given mutex. If it is currently locked any thread, then the calling thread will block until mutex
    /// is released or the specified time elapses. If mutex is recursive, then it can be locked multiple times by the
    /// same thread.
    /// @param timeout      Timeout to wait for the operation.
    /// @return Error code of the operation.
    std::error_code timedLock(Timeout timeout);

    /// Unlocks the given mutex.
    /// @return Error code of the operation.
    /// @note Unlocking mutex that was already locked by another thread invokes undefined behavior.
    std::error_code unlock();

    /// Unlocks the given mutex.
    /// @return Error code of the operation.
    /// @note This function will never block and is supposed to be called from ISR.
    /// @note Unlocking mutex that was already locked by another thread invokes undefined behavior.
    std::error_code unlockIsr();

private:
    OsalMutex m_mutex{};
};

} // namespace osal
