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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

uint64_t osalGetTimestampMs();
uint64_t osalGetTimestampUs();
uint64_t osalGetTimestampNs();

enum ConversionsToSec
{
    eOsalMsInSec = 1000,
    eOsalUsInSec = 1000000,
    eOsalNsInSec = 1000000000
};

enum ConversionsToMs
{
    eOsalUsInMs = 1000,
    eOsalNsInMs = 1000000
};

enum ConversionsToUs
{
    eOsalNsInUs = 1000
};

static inline uint64_t osalMsToSec(uint64_t milliseconds)
{
    return (milliseconds / eOsalMsInSec);
}

static inline uint64_t osalUsToSec(uint64_t microseconds)
{
    return (microseconds / eOsalUsInSec);
}

static inline uint64_t osalNsToSec(uint64_t nanoseconds)
{
    return (nanoseconds / eOsalNsInSec);
}

static inline uint64_t osalSecToMs(uint64_t seconds)
{
    return (seconds * eOsalMsInSec);
}

static inline uint64_t osalUsToMs(uint64_t microseconds)
{
    return (microseconds / eOsalUsInMs);
}

static inline uint64_t osalNsToMs(uint64_t nanoseconds)
{
    return (nanoseconds / eOsalNsInMs);
}

static inline uint64_t osalSecToUs(uint64_t seconds)
{
    return (seconds * eOsalUsInSec);
}

static inline uint64_t osalMsToUs(uint64_t milliseconds)
{
    return (milliseconds * eOsalUsInMs);
}

static inline uint64_t osalNsToUs(uint64_t nanoseconds)
{
    return (nanoseconds / eOsalNsInUs);
}

static inline uint64_t osalSecToNs(uint64_t seconds)
{
    return (seconds * eOsalNsInSec);
}

static inline uint64_t osalMsToNs(uint64_t milliseconds)
{
    return (milliseconds * eOsalNsInMs);
}

static inline uint64_t osalUsToNs(uint64_t microseconds)
{
    return (microseconds * eOsalNsInUs);
}

#ifdef __cplusplus
}
#endif
