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

#include "osal/sleep.hpp"
#include "osal/timestamp.hpp"

#include <cassert>
#include <chrono>
#include <type_traits>

// NOLINTNEXTLINE(google-global-names-in-headers)
using namespace std::chrono_literals;

namespace osal {
namespace detail {

/// Helper type to perform SFINAE to prevent using std::chrono unit in Timeout, which is smaller than Duration,
/// than Duration type.
template <typename T>
using NotLessThanDuration = std::enable_if_t<std::is_same_v<std::common_type_t<T, Duration>, Duration>, bool>;

} // namespace detail

/// Represents an universal timeout object, that can tell if the given timeout has already expired.
/// Upon construction Timeout object calculates timestamp value, which determines deadline for the given operation.
/// Calling "isExpired()" simply compares current timestamp value with the calculated one.
/// @note This class is supposed to replace all occurrences of the raw std::uint32_t timeout values in OSAL and HAL
///       APIs.
class Timeout {
public:
    /// Constructor.
    /// @tparam Representation      Signed arithmetic type representing the number of ticks in the clock's duration.
    /// @tparam Period              A std::ratio type representing the tick period of the clock, in seconds.
    /// @param duration             Maximal time duration in std::chrono duration value, that should last
    ///                             before timeout is considered as expired.
    /// @param forceExpire          Flag forcing timeout to be immediately expired after construction.
    template <typename Representation,
              typename Period,
              typename = detail::NotLessThanDuration<std::chrono::duration<Representation, Period>>>
    Timeout(const std::chrono::duration<Representation, Period>& duration, bool forceExpire = false) // NOLINT
        : m_duration(duration)
        , m_infinity(duration == Duration::max())
    {
        if (!isInfinity())
            m_expireTimestamp = timestamp() + (forceExpire ? Duration::zero() : duration);

        assert(duration >= Duration::zero());
    }

    /// Conversion operator to Duration.
    /// @return Duration value representing time left in std::chrono unit to the deadline timestamp.
    operator Duration() const { return timeLeft(); } // NOLINT

    /// Returns the std::chrono duration value used to initialize timeout.
    /// @return std::chrono duration value used to initialize timeout.
    [[nodiscard]] Duration duration() const { return m_duration; }

    /// Checks, if the deadline has been reached.
    /// @return Boolean flag indicating if the deadline has been reached.
    /// @retval true                 Deadline has been reached.
    /// @retval false                Deadline has not been reached.
    [[nodiscard]] bool isExpired() const { return !isInfinity() && (timeLeft() == Duration::zero()); }

    /// Checks, if given timeout represents infinity.
    /// @return Boolean flag indicating if given timeout represents infinity.
    /// @retval true                 Timeout represents infinity.
    /// @retval false                Timeout does not represent infinity.
    [[nodiscard]] bool isInfinity() const { return m_infinity; }

    /// Returns the std::chrono duration value representing time left to the deadline timestamp.
    /// @return std::chrono duration value representing time left to the deadline timestamp.
    [[nodiscard]] Duration timeLeft() const
    {
        if (isInfinity())
            return Duration::max();

        auto now = timestamp();
        if (now > m_expireTimestamp)
            return Duration::zero();

        return std::chrono::duration_cast<Duration>(m_expireTimestamp - now);
    }

    /// Resets internal state of the timeout. Deadline is recalculated as if timeout was created during this call.
    void reset()
    {
        Timeout other(duration());
        m_expireTimestamp = other.m_expireTimestamp;
    }

    /// Returns helper constant representing zero timeout.
    /// @return Helper constant representing zero timeout.
    static Timeout none() { return {Duration::zero()}; }

    /// Returns helper constant representing infinity timeout.
    /// @return Helper constant representing infinity timeout.
    static Timeout infinity() { return {Duration::max()}; }

private:
    Duration m_duration;
    bool m_infinity;
    std::chrono::time_point<Clock, Duration> m_expireTimestamp;
};

/// Blocks current thread until given timeout is expired.
/// @param timeout                  Timeout to be checked.
static inline void sleepUntilExpired(const Timeout& timeout)
{
    if (!timeout.isExpired())
        sleep(timeout.timeLeft());
}

/// Coverts duration of the given timeout to raw milliseconds.
/// @param timeout                  Timeout to be used.
/// @return Duration of the given timeout expressed in raw milliseconds.
static inline std::uint32_t durationMs(const osal::Timeout& timeout)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(timeout.duration()).count();
}

} // namespace osal
