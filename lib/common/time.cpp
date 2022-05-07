/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2022-2022, Kuba Sejdak <kuba.sejdak@gmail.com>
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

#include "osal/time.h" // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

#include <ctime>
#include <string>

// This declaration is needed, because embedded platforms do not define timegm() function. Platform fills this gap
// by providing its own implementation, but still <ctime> header doesn't provide this prototype.
extern "C" time_t timegm(struct tm* tm); // NOLINT

struct tm osalTimeToTm(time_t value)
{
    tm result{};
    if (gmtime_r(&value, &result) == nullptr)
        return {};

    return result;
}

struct tm osalTimespecToTm(struct timespec value)
{
    return osalTimeToTm(osalTimespecToTime(value));
}

struct tm osalTimevalToTm(struct timeval value)
{
    return osalTimeToTm(osalTimevalToTime(value));
}

time_t osalTmToTime(struct tm value)
{
    return int(timegm(&value));
}

time_t osalTimespecToTime(struct timespec value)
{
    return value.tv_sec;
}

time_t osalTimevalToTime(struct timeval value)
{
    return value.tv_sec;
}

struct timespec osalTimeToTimespec(time_t value)
{
    return {value, 0};
}

struct timespec osalTmToTimespec(struct tm value)
{
    return {osalTmToTime(value), 0};
}

struct timespec osalTimevalToTimespec(struct timeval value)
{
    return {osalTimevalToTime(value), 0};
}

struct timeval osalTimeToTimeval(time_t value)
{
    return {value, 0};
}

struct timeval osalTmToTimeval(struct tm value)
{
    return {osalTmToTime(value), 0};
}

struct timeval osalTimespecToTimeval(struct timespec value)
{
    return {osalTimespecToTime(value), 0};
}

OsalError osalTmToString(struct tm value, char* str, size_t size, OsalTimeStringFormat format)
{
    std::string formatStr;
    std::size_t requiredSize{};

    switch (format) {
        case OsalTimeStringFormat::eTime:
            formatStr = "%T";
            requiredSize = 9; // NOLINT
            break;
        case OsalTimeStringFormat::eDate:
            formatStr = "%d.%m.%Y";
            requiredSize = 11; // NOLINT
            break;
        case OsalTimeStringFormat::eTimeDate:
            formatStr = "%T %d.%m.%Y";
            requiredSize = 20; // NOLINT
            break;
        case OsalTimeStringFormat::eSortedDateTime:
            formatStr = "%Y%m%d_%H%M%S";
            requiredSize = 16; // NOLINT
            break;
    }

    if (size < requiredSize)
        return OsalError::eInvalidArgument;

    (void) std::strftime(str, requiredSize, formatStr.c_str(), &value);
    return OsalError::eOk;
}

OsalError osalTimeToString(time_t value, char* str, size_t size, OsalTimeStringFormat format)
{
    return osalTmToString(osalTimeToTm(value), str, size, format);
}

OsalError osalTimespecToString(struct timespec value, char* str, size_t size, OsalTimeStringFormat format)
{
    return osalTmToString(osalTimespecToTm(value), str, size, format);
}

OsalError osalTimevalToString(struct timeval value, char* str, size_t size, OsalTimeStringFormat format)
{
    return osalTmToString(osalTimevalToTm(value), str, size, format);
}
