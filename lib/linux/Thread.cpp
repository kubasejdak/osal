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

#include "osal/Thread.h"

#include <pthread.h>
#include <sched.h>

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstring>
#include <memory>

/// Represents helper wrapper around OSAL thread function and its arguments.
/// @note This type is necessary, because OsalThreadFunction has different signature than pthread.
///       Thus special threadWrapper() function (with pthread compliant signature) is used directly in
///       call to pthread_create() and OSAL thread function is passed along with its arguments as the
///       argument.
struct ThreadWrapperData {
    OsalThreadFunction func{};
    void* param{};
};

/// Helper thread function that has signature required by pthread. It is used as a wrapper for
/// OSAL thread function.
/// @param arg          Helper thread arguments.
/// @return Result of this function is never used so it always returns nullptr.
static void* threadWrapper(void* arg)
{
    auto wrapperData = std::unique_ptr<ThreadWrapperData>(static_cast<ThreadWrapperData*>(arg));
    wrapperData->func(wrapperData->param);
    return nullptr;
}

OsalError osalThreadCreate(OsalThread* thread, OsalThreadConfig config, OsalThreadFunction func, void* arg)
{
    if (thread == nullptr || func == nullptr)
        return OsalError::eInvalidArgument;

    thread->initialized = false;

    const auto cPriorityMin = sched_get_priority_min(SCHED_RR);
    const auto cPriorityMax = sched_get_priority_max(SCHED_RR);
    const auto cPriorityStep = (cPriorityMax - cPriorityMin) / 4;

    int priority{};
    switch (config.priority) {
        case OsalThreadPriority::eLowest: priority = cPriorityMin; break;
        case OsalThreadPriority::eLow: priority = cPriorityMin + (cPriorityStep * 1); break;
        case OsalThreadPriority::eNormal: priority = cPriorityMin + (cPriorityStep * 2); break;
        case OsalThreadPriority::eHigh: priority = cPriorityMin + (cPriorityStep * 3); break;
        case OsalThreadPriority::eHighest: priority = cPriorityMax; break;
        default: return OsalError::eInvalidArgument;
    }

    pthread_attr_t attr{};
    [[maybe_unused]] auto result = pthread_attr_init(&attr);
    assert(result == 0);

    result = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    assert(result == 0);

    sched_param schedParam{};
    schedParam.sched_priority = priority;
    result = pthread_attr_setschedparam(&attr, &schedParam);
    assert(result == 0);

    auto stackSize = std::max<std::size_t>(config.stackSize, PTHREAD_STACK_MIN);
    result = pthread_attr_setstacksize(&attr, stackSize);
    assert(result == 0);

    pthread_t handle{};
    auto wrapper = std::make_unique<ThreadWrapperData>();
    wrapper->func = func;
    wrapper->param = arg;
    result = pthread_create(&handle, &attr, threadWrapper, wrapper.release());
    assert(result == 0);

    result = pthread_attr_destroy(&attr);
    assert(result == 0);

    thread->impl.handle = handle;
    thread->initialized = true;
    return OsalError::eOk;
}

OsalError osalThreadDestroy(OsalThread* thread)
{
    if (thread == nullptr || !thread->initialized)
        return OsalError::eInvalidArgument;

    std::memset(thread, 0, sizeof(OsalThread));
    return OsalError::eOk;
}

OsalError osalThreadJoin(OsalThread* thread)
{
    if (thread == nullptr || !thread->initialized)
        return OsalError::eInvalidArgument;

    if (pthread_join(thread->impl.handle, nullptr) != 0)
        return OsalError::eOsError;

    return OsalError::eOk;
}

void osalThreadYield()
{
    sched_yield();
}

uint32_t osalThreadId()
{
    return std::hash<pthread_t>{}(pthread_self());
}
