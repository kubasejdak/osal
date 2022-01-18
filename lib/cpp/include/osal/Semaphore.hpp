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

#include "osal/Error.hpp"
#include "osal/Semaphore.h"
#include "osal/Timeout.hpp"

#include <system_error>

namespace osal {

/// Represents OSAL semaphore handle.
class Semaphore {
public:
    /// Constructor. Creates new semaphore with the given type.
    /// @param initialValue     Initial value of the semaphore to be created.
    explicit Semaphore(unsigned int initialValue);

    /// Copy constructor.
    /// @note This constructor is deleted, because Semaphore is not meant to be copy-constructed.
    Semaphore(const Semaphore&) = delete;

    /// Move constructor.
    /// @param other            Object to be moved from.
    Semaphore(Semaphore&& other) noexcept;

    /// Destructor.
    ~Semaphore();

    /// Copy assignment operator.
    /// @return Reference to self.
    /// @note This operator is deleted, because Semaphore is not meant to be copy-assigned.
    Semaphore& operator=(const Semaphore&) = delete;

    /// Move assignment operator.
    /// @return Reference to self.
    /// @note This operator is deleted, because Semaphore is not meant to be move-assigned.
    Semaphore& operator=(Semaphore&&) = delete;

    /// Decrements value of the given semaphore. If its value is currently 0, then the calling thread will block until
    /// semaphore is positive again.
    /// @return Error code of the operation.
    std::error_code wait();

    /// Decrements value of the given semaphore. If its value is currently 0, then it returns immediately
    /// with a proper error.
    /// @return Error code of the operation.
    std::error_code tryWait();

    /// Decrements value of the given semaphore. If its value is currently 0, then it returns immediately
    /// with a proper error.
    /// @return Error code of the operation.
    /// @note This function will never block and is supposed to be called from ISR.
    std::error_code tryWaitIsr();

    /// Decrements value of the given semaphore. If its value is currently 0, then the calling thread will block until
    /// semaphore is positive again or the specified time elapses.
    /// @param timeout          Maximal time to wait for the operation.
    /// @return Error code of the operation.
    std::error_code timedWait(Timeout timeout);

    /// Increments value of the given semaphore.
    /// @return Error code of the operation.
    /// @note There is no upper bound of the semaphore value.
    std::error_code signal();

    /// Increments value of the given semaphore.
    /// @return Error code of the operation.
    /// @note This function will never block and is supposed to be called from ISR.
    /// @note There is no upper bound of the semaphore value.
    std::error_code signalIsr();

private:
    OsalSemaphore m_semaphore{};
};

} // namespace osal
