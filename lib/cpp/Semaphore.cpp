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

#include "osal/Semaphore.hpp"

#include <chrono>

namespace osal {

Semaphore::Semaphore(unsigned int initialValue)
{
    osalSemaphoreCreate(&m_semaphore, initialValue);
}

Semaphore::Semaphore(Semaphore&& other) noexcept
{
    std::swap(m_semaphore, other.m_semaphore);
}

Semaphore::~Semaphore()
{
    if (m_semaphore.initialized)
        osalSemaphoreDestroy(&m_semaphore);
}

std::error_code Semaphore::wait()
{
    return osalSemaphoreWait(&m_semaphore);
}

std::error_code Semaphore::tryWait()
{
    return osalSemaphoreTryWait(&m_semaphore);
}

std::error_code Semaphore::tryWaitIsr()
{
    return osalSemaphoreTryWaitIsr(&m_semaphore);
}

std::error_code Semaphore::timedWait(Timeout timeout)
{
    std::uint32_t timeoutMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeout.timeLeft()).count();
    return osalSemaphoreTimedWait(&m_semaphore, timeoutMs);
}

std::error_code Semaphore::signal()
{
    return osalSemaphoreSignal(&m_semaphore);
}

std::error_code Semaphore::signalIsr()
{
    return osalSemaphoreSignalIsr(&m_semaphore);
}

} // namespace osal
