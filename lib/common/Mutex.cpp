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

#include "osal/Mutex.hpp"

#include "osal/Error.hpp"

#include <mutex>
#include <thread>

namespace osal {
namespace detail {

struct MutexImpl {
    std::recursive_timed_mutex mutex;
    std::mutex helperMutex;
    std::thread::id owner;
};

} // namespace detail

Mutex::Mutex(MutexType type) noexcept
    : m_type(type)
    , m_impl(std::make_unique<detail::MutexImpl>())
{}

Mutex::~Mutex() = default;

std::error_code Mutex::lock()
{
    std::lock_guard<std::mutex> lock(m_impl->helperMutex);
    if (m_type == MutexType::eNonRecursive && m_lockCounter == 1) {
        if (m_impl->owner == std::this_thread::get_id())
            return Error::eRecursiveUsage;
    }

    m_impl->mutex.lock();

    m_impl->owner = std::this_thread::get_id();
    ++m_lockCounter;
    return Error::eOk;
}

std::error_code Mutex::lock(Timeout timeout)
{
    (void) timeout;
    (void) m_impl;
    return Error::eOk;
}

std::error_code Mutex::tryLock()
{
    std::lock_guard<std::mutex> lock(m_impl->helperMutex);
    if (m_type == MutexType::eNonRecursive && m_lockCounter == 1) {
        if (m_impl->owner == std::this_thread::get_id())
            return Error::eRecursiveUsage;
    }

    if (!m_impl->mutex.try_lock())
        return Error::eLocked;

    m_impl->owner = std::this_thread::get_id();
    ++m_lockCounter;
    return Error::eOk;
}

std::error_code Mutex::unlock()
{
    std::lock_guard<std::mutex> lock(m_impl->helperMutex);
    if (m_impl->owner != std::this_thread::get_id())
        return Error::eNotOwner;

    if (m_lockCounter == 0)
        return Error::eNotLocked;

    m_impl->mutex.unlock();

    --m_lockCounter;
    m_impl->owner = {};
    return Error::eOk;
}

} // namespace osal
