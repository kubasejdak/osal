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

#include "osal/thread.h"

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <memory>

struct ThreadWrapperData {
    OsalThreadFunction func{};
    void* param{};
};

static void* threadWrapper(void* arg)
{
    auto* wrapperData = static_cast<ThreadWrapperData*>(arg);
    wrapperData->func(wrapperData->param);

    std::free(wrapperData); // NOLINT
    return nullptr;
}

OsalError osalThreadCreate(OsalThread* thread, OsalThreadConfig config, OsalThreadFunction func, void* arg)
{
    if (thread == nullptr || func == nullptr || thread->impl.initialized)
        return OsalError::eInvalidArgument;

    thread->impl.initialized = false;

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
    if (pthread_attr_init(&attr) != 0)
        return OsalError::eOsError;

    if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0)
        return OsalError::eOsError;

    sched_param schedParam{};
    schedParam.sched_priority = priority;
    if (pthread_attr_setschedparam(&attr, &schedParam) != 0)
        return OsalError::eOsError;

    auto stackSize = std::max<std::size_t>(config.stackSize, PTHREAD_STACK_MIN);
    if (pthread_attr_setstacksize(&attr, stackSize) != 0)
        return OsalError::eOsError;

    pthread_t handle{};
    auto wrapper = std::make_unique<ThreadWrapperData>();
    wrapper->func = func;
    wrapper->param = arg;
    if (pthread_create(&handle, &attr, threadWrapper, wrapper.release()) != 0)
        return OsalError::eOsError;

    if (pthread_attr_destroy(&attr) != 0)
        return OsalError::eOsError;

    thread->impl.handle = handle;
    thread->impl.initialized = true;
    return OsalError::eOk;
}

OsalError osalThreadDestroy(OsalThread* thread)
{
    if (thread == nullptr || !thread->impl.initialized)
        return OsalError::eInvalidArgument;

    std::memset(&thread->impl, 0, sizeof(thread->impl));
    return OsalError::eOk;
}

OsalError osalThreadJoin(OsalThread* thread)
{
    if (thread == nullptr || !thread->impl.initialized)
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
