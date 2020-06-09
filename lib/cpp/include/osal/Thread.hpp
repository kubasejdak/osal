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
#include "osal/Thread.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>
#include <utility>

namespace osal {

/// Represents OSAL thread handle.
/// @tparam cPriority           Priority to be used in thread construction.
/// @tparam cStackSize          Stack size to be used in thread construction.
/// @note This type can be used to start thread with any function (with any signature), in contrary to the C version
///       of this API.
template <OsalThreadPriority cPriority = cOsalThreadDefaultPriority,
          std::size_t cStackSize = cOsalThreadDefaultStackSize>
class Thread {
public:
    /// Default constructor.
    /// @note This constructor creates an empty thread. It can be later started with start() method.
    Thread() = default;

    /// Constructor.
    /// @tparam ThreadFunction  Type of user function to be invoked by the new thread.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @note This constructor immediately starts the thread.
    template <typename ThreadFunction, typename... Args>
    explicit Thread(ThreadFunction function, Args&&... args)
    {
        start(std::forward<ThreadFunction>(function), std::forward<Args>(args)...);
    }

    /// Copy constructor.
    /// @note This constructor is deleted, because Thread is not meant to be copy-constructed.
    Thread(const Thread&) = delete;

    /// Move constructor.
    /// @param other            Thread instance to be moved from.
    Thread(Thread&& other) noexcept
    {
        std::swap(m_thread, other.m_thread);
        std::swap(m_stack, other.m_stack);
        std::swap(m_userFunction, other.m_userFunction);
        std::swap(m_workerFunction, other.m_workerFunction);
        std::swap(m_started, other.m_started);
    }

    /// Destructor.
    /// @note This destructor automatically joins and destroys the underlying thread. However thread can be
    ///       independently joined by the user.
    ~Thread()
    {
        osalThreadJoin(&m_thread);
        osalThreadDestroy(&m_thread);
    }

    /// Copy assignment operator.
    /// @note This operator is deleted, because Thread is not meant to be copy-assigned.
    Thread& operator=(const Thread&) = delete;

    /// Move assignment operator.
    /// @note This operator is deleted, because Thread is not meant to be move-assigned.
    Thread& operator=(Thread&& other) = delete;

    /// Sets custom stack to be used by the created Thread.
    /// @param stack            Custom stack to be used by the Thread.
    /// @return Error code of the operation.
    std::error_code setStack(void* stack)
    {
        if (stack == nullptr)
            return OsalError::eInvalidArgument;

        m_stack = stack;
        return OsalError::eOk;
    }

    /// Starts the thread.
    /// @tparam ThreadFunction  Type of user function to be invoked by the new thread.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param stack            Pointer to the stack to be used by this thread.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @return Error code of the operation.
    template <typename ThreadFunction, typename... Args>
    std::error_code start(ThreadFunction function, Args&&... args)
    {
        if (m_started)
            return OsalError::eThreadAlreadyStarted;

        m_userFunction = std::make_unique<FunctionWrapper>(
            // NOLINTNEXTLINE(modernize-avoid-bind)
            std::bind(std::forward<ThreadFunction>(function), std::forward<Args>(args)...));
        assert(m_userFunction);

        m_workerFunction = [](void* arg) {
            auto userFunction = *static_cast<FunctionWrapper*>(arg);
            userFunction();
        };

        auto error
            = osalThreadCreate(&m_thread, {cPriority, cStackSize, m_stack}, m_workerFunction, m_userFunction.get());

        m_started = (error == OsalError::eOk);
        return error;
    }

    /// Joins the thread represented by the given handle. If thread has not been terminated yet, then this call blocks
    /// the caller until given thread is finished.
    /// @return Error code of the operation.
    std::error_code join() { return osalThreadJoin(&m_thread); }

private:
    /// Helper wrapper around user thread function.
    using FunctionWrapper = std::function<void(void)>;

    OsalThread m_thread{};
    void* m_stack{};
    std::unique_ptr<FunctionWrapper> m_userFunction;
    OsalThreadFunction m_workerFunction{};
    bool m_started{};
};

/// Helper type alias representing OSAL thread with OsalThreadPriority::eLowest priority.
/// @tparam cStackSize          Stack size to be used in thread construction.
template <std::size_t cStackSize = cOsalThreadDefaultStackSize>
using LowestPrioThread = Thread<OsalThreadPriority::eLowest, cStackSize>;

/// Helper type alias representing OSAL thread with OsalThreadPriority::eLow priority.
/// @tparam cStackSize          Stack size to be used in thread construction.
template <std::size_t cStackSize = cOsalThreadDefaultStackSize>
using LowPrioThread = Thread<OsalThreadPriority::eLow, cStackSize>;

/// Helper type alias representing OSAL thread with OsalThreadPriority::eNormal priority.
/// @tparam cStackSize          Stack size to be used in thread construction.
template <std::size_t cStackSize = cOsalThreadDefaultStackSize>
using NormalPrioThread = Thread<OsalThreadPriority::eNormal, cStackSize>;

/// Helper type alias representing OSAL thread with OsalThreadPriority::eHigh priority.
/// @tparam cStackSize          Stack size to be used in thread construction.
template <std::size_t cStackSize = cOsalThreadDefaultStackSize>
using HighPrioThread = Thread<OsalThreadPriority::eHigh, cStackSize>;

/// Helper type alias representing OSAL thread with OsalThreadPriority::eHighest priority.
/// @tparam cStackSize          Stack size to be used in thread construction.
template <std::size_t cStackSize = cOsalThreadDefaultStackSize>
using HighestPrioThread = Thread<OsalThreadPriority::eHighest, cStackSize>;

namespace thread {

/// Invokes context switch in the scheduler on demand.
/// @note It is up to the scheduler which thread will be selected to be executed next. It is possible, that
///       it will be the same thread which called this function.
inline void yield()
{
    osalThreadYield();
}

/// Returns numerical id of the current thread.
/// @note It is up to the concrete implementation what this number means. The only thing that caller can depend on
///       is that on the given platform this value will be unique among all created threads.
[[nodiscard]] inline std::uint32_t id()
{
    return osalThreadId();
}

} // namespace thread
} // namespace osal
