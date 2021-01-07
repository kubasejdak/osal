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

#include "osal/timestamp.h"

#include <chrono>

/// Internal value of system time latched during OSAL initialization.
/// @note This value must be properly set by OSAL initialization in order to have correct values
/// returned from timestamp module.
// NOLINTNEXTLINE(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
std::chrono::steady_clock::time_point initTime;

/// Returns the time since the osalInit() function was called.
/// @tparam Unit        Unit in which time should be represented (converted to).
/// @return Returns the time since the osalInit() function was called.
template <typename Unit>
static std::uint64_t timeSinceStart()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<Unit>(now - initTime).count();
}

uint64_t osalTimestampMs()
{
    return timeSinceStart<std::chrono::milliseconds>();
}

uint64_t osalTimestampUs()
{
    return timeSinceStart<std::chrono::microseconds>();
}

uint64_t osalTimestampNs()
{
    return timeSinceStart<std::chrono::nanoseconds>();
}
