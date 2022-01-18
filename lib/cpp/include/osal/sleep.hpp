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

#include <chrono>
#include <cstdint>

// NOLINTNEXTLINE(google-global-names-in-headers)
using namespace std::chrono_literals;

namespace osal {
namespace detail {

/// Internal function implementing thread suspension for time intervals in ms.
/// @param durationMs           Time to sleep in ms.
void sleepMs(std::uint64_t durationMs);

} // namespace detail

/// Suspends the current thread for the specified amount of time.
/// @tparam Representation      Signed arithmetic type representing the number of ticks in the clock's duration.
/// @tparam Period              A std::ratio type representing the tick period of the clock, in seconds.
/// @param duration             Amount of time for which current thread should be suspended.
/// @note This function can accept any time duration unit supported by std::chrono library.
template <typename Representation, typename Period>
void sleep(const std::chrono::duration<Representation, Period>& duration)
{
    detail::sleepMs(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

} // namespace osal
