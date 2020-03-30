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

template <typename ThreadFunction,
          OsalThreadPriority priority = cOsalThreadDefaultPriority,
          std::size_t stackSize = cOsalThreadDefaultStackSize>
class Thread {
public:
    Thread() = default;

    template <typename... Args>
    explicit Thread(ThreadFunction function, Args&&... args)
        : Thread(nullptr, function, std::forward<Args>(args)...)
    {}

    template <typename... Args>
    Thread(void* stack, ThreadFunction function, Args&&... args)
    {
        start(stack, function, std::forward<Args>(args)...);
    }

    Thread(const Thread&) = delete;

    Thread(Thread&& other) noexcept
    {
        std::swap(m_thread, other.m_thread);
        std::swap(m_userFunction, other.m_userFunction);
        std::swap(m_workerFunction, other.m_workerFunction);
        std::swap(m_started, other.m_started);
    }

    ~Thread() { osalThreadDestroy(&m_thread); }

    Thread& operator=(const Thread&) = delete;

    Thread& operator=(Thread&& other) = delete;

    template <typename... Args>
    std::error_code start(ThreadFunction function, Args&&... args)
    {
        return start(nullptr, function, std::forward<Args>(args)...);
    }

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

        auto error = osalThreadCreate(&m_thread, {priority, stackSize, stack}, m_workerFunction, m_userFunction.get());

        m_started = (error == OsalError::eOk);
        return error;
    }

    std::error_code join() { return osalThreadJoin(&m_thread); }

    static void yield() { osalThreadYield(); }

    [[nodiscard]] static std::uint32_t id() { return osalThreadId(); }

private:
    using FunctionWrapper = std::function<void(void)>;

    OsalThread m_thread{};
    std::unique_ptr<FunctionWrapper> m_userFunction;
    OsalThreadFunction m_workerFunction{};
    bool m_started{};
};

} // namespace osal
