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

#ifdef __cplusplus
extern "C" {
#endif

#include "Error.h"
#include "internal/ThreadImpl.h"

#include <stddef.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)
#include <stdint.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

/// Represents OSAL thread handle.
/// @note Size of this structure depends on the concrete implementation. In particular, ThreadImpl
///       contains objects from the target platform. Thus depending on its size is not recommended.
struct OsalThread {
    ThreadImpl impl;
    bool initialized;
};

/// Represents possible thread priorities that can be set with the OSAL API.
/// @note It is up to the concrete implementation which physical priorities will be used for each
///       enum value. The only assumption client can make, is that eLowest will set the lowest
//        possible priority in the system and eHighest will set the highest possible priority in the system.
///       All implementations will also try to set eNormal to the default priority on the target system.
enum OsalThreadPriority {
    eLowest,
    eLow,
    eNormal,
    eHigh,
    eHighest
};

/// Helper constant with default thread priority of new thread.
static const OsalThreadPriority cOsalThreadDefaultPriority = OsalThreadPriority::eNormal;

/// Helper constant with default stack size of new thread.
static const size_t cOsalThreadDefaultStackSize = 8 * 1024;

/// Represents structure used to configuration for created thread.
/// @note Stack is not used in all configurations (e.g. Linux doesn't support it).
struct OsalThreadConfig {
    OsalThreadPriority priority;
    size_t stackSize;
    void* stack;
};

/// Represents signature of the user function that can be invoked by OSAL thread.
typedef void (*OsalThreadFunction)(void*); // NOLINT(modernize-use-using)

/// Creates and immediately starts new thread with the following configuration, main function and arguments.
/// @param thread           Thread handle to be initialized.
/// @param config           OSAL thread configuration to be used to setup new thread.
/// @param func             User function to be invoked by the new thread.
/// @param arg              User argument to be passed to the used function.
/// @return Error code of the operation.
/// @note Argument is passed as pointer, which means that it comes from the stack, then its lifetime must be
///       at least the same as created thread.
OsalError osalThreadCreate(OsalThread* thread, OsalThreadConfig config, OsalThreadFunction func, void* arg);

/// Destroys thread represented by the given handle.
/// @param thread           Thread handle to be destroyed.
/// @return Error code of the operation.
/// @note Destroying thread that has not been joined can invoke undefined behavior.
OsalError osalThreadDestroy(OsalThread* thread);

/// Joins the thread represented by the given handle. If thread has not been terminated yet, then this call blocks
/// the caller until given thread is finished.
/// @param thread           Thread handle to be destroyed.
/// @return Error code of the operation.
OsalError osalThreadJoin(OsalThread* thread);

/// Invokes context switch in the scheduler on demand.
/// @note It is up to the scheduler which thread will be selected to be executed next. It is possible, that
///       it will be the same thread which called this function.
void osalThreadYield();

/// Returns numerical id of the current thread.
/// @note It is up to the concrete implementation what this number means. The only thing that caller can depend on
///       is that on the given platform this value will be unique among all created threads.
uint32_t osalThreadId();

#ifdef __cplusplus
}
#endif
