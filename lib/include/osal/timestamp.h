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

#include <stdint.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

/// Returns the timestamp relative to the call to osalInit() function in ms.
/// @return Timestamp relative to the call to osalInit() function in ms.
/// @note osalInit() has to be called in order to have correct values returned by this function.
uint64_t osalTimestampMs();

/// Returns the timestamp relative to the call to osalInit() function in us.
/// @return Timestamp relative to the call to osalInit() function in us.
/// @note osalInit() has to be called in order to have correct values returned by this function.
uint64_t osalTimestampUs();

/// Returns the timestamp relative to the call to osalInit() function in ns.
/// @return Timestamp relative to the call to osalInit() function in ns.
/// @note osalInit() has to be called in order to have correct values returned by this function.
uint64_t osalTimestampNs();

/// Converts milliseconds to seconds.
/// @param milliseconds         Milliseconds to be converted.
/// @return Milliseconds expressed in a number of seconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalMsToSec(uint64_t milliseconds);

/// Converts microseconds to seconds.
/// @param microseconds         Microseconds to be converted.
/// @return Microseconds expressed in a number of seconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalUsToSec(uint64_t microseconds);

/// Converts nanoseconds to seconds.
/// @param nanoseconds          Nanoseconds to be converted.
/// @return Nanoseconds expressed in a number of seconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalNsToSec(uint64_t nanoseconds);

/// Converts seconds to milliseconds.
/// @param seconds              Seconds to be converted.
/// @return Seconds expressed in a number of milliseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalSecToMs(uint64_t seconds);

/// Converts microseconds to milliseconds.
/// @param microseconds         Microseconds to be converted.
/// @return Microseconds expressed in a number of milliseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalUsToMs(uint64_t microseconds);

/// Converts nanoseconds to milliseconds.
/// @param nanoseconds          Nanoseconds to be converted.
/// @return Nanoseconds expressed in a number of milliseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalNsToMs(uint64_t nanoseconds);

/// Converts seconds to microseconds.
/// @param seconds              Seconds to be converted.
/// @return Seconds expressed in a number of microseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalSecToUs(uint64_t seconds);

/// Converts milliseconds to microseconds.
/// @param milliseconds         Milliseconds to be converted.
/// @return Milliseconds expressed in a number of microseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalMsToUs(uint64_t milliseconds);

/// Converts nanoseconds to microseconds.
/// @param nanoseconds          Nanoseconds to be converted.
/// @return Nanoseconds expressed in a number of microseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalNsToUs(uint64_t nanoseconds);

/// Converts seconds to nanoseconds.
/// @param seconds              Seconds to be converted.
/// @return Seconds expressed in a number of nanoseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalSecToNs(uint64_t seconds);

/// Converts milliseconds to nanoseconds.
/// @param milliseconds         Milliseconds to be converted.
/// @return Milliseconds expressed in a number of nanoseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalMsToNs(uint64_t milliseconds);

/// Converts microseconds to nanoseconds.
/// @param microseconds         Microseconds to be converted.
/// @return Microseconds expressed in a number of nanoseconds.
/// @note This function doesn't check for overflows or conversion loses.
uint64_t osalUsToNs(uint64_t microseconds);

#ifdef __cplusplus
}
#endif
