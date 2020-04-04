/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2019-2020, Kuba Sejdak <kuba.sejdak@gmail.com>
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

#include "osal/error.hpp"
#include "osal/thread.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>
#include <utility>

namespace osal {

/// @class Thread
/// Represents OSAL thread handle.
/// @tparam ThreadFunction      Type of user function to be invoked by the new thread.
/// @tparam cPriority           Priority to be used in thread construction.
/// @tparam cStackSize          Stack size to be used in thread construction.
/// @note This type can be used to start thread with any function (with any signature), in contrary to the C version
///       of this API.
template <typename ThreadFunction = OsalThreadFunction,
          OsalThreadPriority cPriority = cOsalThreadDefaultPriority,
          std::size_t cStackSize = cOsalThreadDefaultStackSize>
class Thread {
public:
    /// Default constructor.
    /// @note This constructor creates an empty thread. It can be later started with start() method.
    Thread() = default;

    /// Constructor.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @note This constructor immediately starts the thread.
    template <typename... Args>
    explicit Thread(ThreadFunction function, Args&&... args)
        : Thread(nullptr, function, std::forward<Args>(args)...)
    {}

    /// Constructor.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param stack            Pointer to the stack to be used by this thread.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @note This constructor immediately starts the thread.
    template <typename... Args>
    Thread(void* stack, ThreadFunction function, Args&&... args)
    {
        start(stack, function, std::forward<Args>(args)...);
    }

    /// Constructor.
    /// @param function         User function to be invoked by the new thread.
    /// @note This constructor immediately starts the thread.
    explicit Thread(ThreadFunction function)
        : Thread(nullptr, function)
    {}

    /// Constructor.
    /// @param stack            Pointer to the stack to be used by this thread.
    /// @param function         User function to be invoked by the new thread.
    /// @note This constructor immediately starts the thread.
    Thread(void* stack, ThreadFunction function) { start(stack, function); }

    /// Copy constructor.
    /// @note This constructor is deleted, because this type is not meant to be copied.
    Thread(const Thread&) = delete;

    /// Move constructor.
    /// @param other            Thread instance to be moved from.
    Thread(Thread&& other) noexcept
    {
        std::swap(m_thread, other.m_thread);
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
    /// @note This operator is deleted, because this type is not meant to be copied.
    Thread& operator=(const Thread&) = delete;

    /// Move assignment operator.
    /// @note This operator is deleted, because this type is not meant to be moved after default construction.
    Thread& operator=(Thread&& other) = delete;

    /// Starts the thread.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @return Error code of the operation.
    template <typename... Args>
    std::error_code start(ThreadFunction function, Args&&... args)
    {
        return start(nullptr, function, std::forward<Args>(args)...);
    }

    /// Starts the thread.
    /// @tparam Args            Types of user arguments to be passed to the used function.
    /// @param stack            Pointer to the stack to be used by this thread.
    /// @param function         User function to be invoked by the new thread.
    /// @param args             User arguments to be passed to the used function.
    /// @return Error code of the operation.
    template <typename... Args>
    std::error_code start(void* stack, ThreadFunction function, Args&&... args)
    {
        if (m_started)
            return OsalError::eThreadAlreadyStarted;

        m_userFunction = std::make_unique<FunctionWrapper>(
            std::bind(std::forward<ThreadFunction>(function), std::forward<Args>(args)...));
        if (m_userFunction == nullptr)
            return OsalError::eInvalidArgument;

        m_workerFunction = [](void* arg) {
            auto threadFunction = *static_cast<FunctionWrapper*>(arg);
            threadFunction();
        };

        auto error
            = osalThreadCreate(&m_thread, {cPriority, cStackSize, stack}, m_workerFunction, m_userFunction.get());

        m_started = (error == OsalError::eOk);
        return error;
    }

    /// Starts the thread.
    /// @param function         User function to be invoked by the new thread.
    /// @return Error code of the operation.
    std::error_code start(ThreadFunction function) { return start(nullptr, function); }

    /// Starts the thread.
    /// @param stack            Pointer to the stack to be used by this thread.
    /// @param function         User function to be invoked by the new thread.
    /// @return Error code of the operation.
    std::error_code start(void* stack, ThreadFunction function)
    {
        if (m_started)
            return OsalError::eThreadAlreadyStarted;

        m_userFunction = std::make_unique<FunctionWrapper>(function);
        if (m_userFunction == nullptr)
            return OsalError::eInvalidArgument;

        m_workerFunction = [](void* arg) {
            auto threadFunction = *static_cast<FunctionWrapper*>(arg);
            threadFunction();
        };

        auto error
            = osalThreadCreate(&m_thread, {cPriority, cStackSize, stack}, m_workerFunction, m_userFunction.get());

        m_started = (error == OsalError::eOk);
        return error;
    }

    /// Joins the thread represented by the given handle. If thread has not been terminated yet, then this call blocks
    /// the caller until given thread is finished.
    /// @return Error code of the operation.
    std::error_code join() { return osalThreadJoin(&m_thread); }

private:
    using FunctionWrapper = std::function<void(void)>;

    OsalThread m_thread{};
    std::unique_ptr<FunctionWrapper> m_userFunction;
    OsalThreadFunction m_workerFunction{};
    bool m_started{};
};

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
