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

#include "osal/Error.hpp"
#include "osal/Mutex.hpp"
#include "osal/Timeout.hpp"

#include <system_error>

namespace osal {

/// Helper RAII type to perform automatic lock/unlock on the specified mutex.
/// @note Objects of this class should not be shared across threads.
class ScopedLock {
public:
    /// Constructor.
    /// @param mutex        Mutex for which all the operations should be performed
    /// @note This constructor automatically locks the underlying mutex.
    explicit ScopedLock(Mutex& mutex);

    /// Constructor.
    /// @param mutex        Mutex for which all the operations should be performed
    /// @param timeout      Timeout to wait for the operation.
    /// @note This constructor automatically locks the underlying mutex.
    ScopedLock(Mutex& mutex, Timeout timeout);

    /// Copy constructor.
    /// @note This constructor is deleted, because ScopedLock is not meant to be copy-constructed.
    ScopedLock(const ScopedLock&) = delete;

    /// Move constructor.
    /// @note This constructor is deleted, because ScopedLock is not meant to be move-constructed.
    ScopedLock(ScopedLock&& other) noexcept = delete;

    /// Destructor.
    /// @note Destructor automatically unlocks the underlying mutex.
    ~ScopedLock();

    /// Copy assignment operator.
    /// @note This operator is deleted, because ScopedLock is not meant to be copy-assigned.
    ScopedLock& operator=(const ScopedLock&) = delete;

    /// Move assignment operator.
    /// @note This operator is deleted, because ScopedLock is not meant to be move-assigned.
    ScopedLock& operator=(ScopedLock&&) = delete;

    /// Returns flag indicating if the underlying mutex is locked.
    /// @return Flag indicating if the underlying mutex is locked.
    /// @retval true        Underlying mutex is locked.
    /// @retval false       Underlying mutex is not locked.
    operator bool() const { return isAcquired(); } // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    /// Returns flag indicating if the underlying mutex is locked.
    /// @return Flag indicating if the underlying mutex is locked.
    /// @retval true        Underlying mutex is locked.
    /// @retval false       Underlying mutex is not locked.
    [[nodiscard]] bool isAcquired() const { return m_locked; }

private:
    /// Locks the underlying mutex. If it is currently locked any thread, then the calling thread will block until mutex
    /// is released. If mutex is recursive, then it can be locked multiple times by the same thread.
    /// @return Error code of the operation.
    /// @note If mutex is non-recursive, then calling this function twice by the same thread will result in a deadlock.
    std::error_code lock();

    /// Locks the underlying mutex. If it is currently locked any thread, then the calling thread will block until mutex
    /// is released or the specified time elapses. If mutex is recursive, then it can be locked multiple times by the
    /// same thread.
    /// @param timeout      Timeout to wait for the operation.
    /// @return Error code of the operation.
    std::error_code timedLock(Timeout timeout);

    /// Unlocks the given mutex.
    /// @return Error code of the operation.
    /// @note Unlocking mutex that was already locked by another thread invokes undefined behavior.
    std::error_code unlock();

private:
    Mutex& m_mutex;
    bool m_locked{};
};

} // namespace osal
