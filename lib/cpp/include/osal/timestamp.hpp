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

#include <chrono>

namespace osal {

/// @typedef Clock
/// Default clock type used by OSAL to represent the timestamp.
using Clock = std::chrono::steady_clock;

/// @typedef Duration
/// Default duration type used by OSAL to represent the timestamp.
using Duration = std::chrono::nanoseconds;

/// Returns the timestamp relative to the call to osal::init() function in ns.
/// @return Timestamp relative to the osal::init() function in ns.
/// @note osal::init() has to be called in order to have correct values returned by this function.
/// @note Timestamp can be easily converted to any unit with std::chrono::duration_cast().
std::chrono::time_point<Clock, Duration> timestamp();

} // namespace osal
