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

#pragma once

#include "osal/time.h" // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)

#include <ctime>
#include <string>

namespace osal {

/// Converts std::time_t value to std::tm.
/// @param value            Value to be converted.
/// @return std::tm created from std::time_t.
std::tm toTm(std::time_t value);

/// Converts std::timespec value to std::tm.
/// @param value            Value to be converted.
/// @return std::tm created from std::timespec.
std::tm toTm(timespec value);

/// Converts std::timeval value to std::tm.
/// @param value            Value to be converted.
/// @return std::tm created from std::timeval.
std::tm toTm(timeval value);

/// Converts std::tm value to std::time_t.
/// @param value            Value to be converted.
/// @return std::time_t created from std::tm.
std::time_t toTime(std::tm value);

/// Converts std::timespec value to std::time_t.
/// @param value            Value to be converted.
/// @return std::time_t created from std::timespec.
std::time_t toTime(timespec value);

/// Converts std::timeval value to std::time_t.
/// @param value            Value to be converted.
/// @return std::time_t created from std::timeval.
std::time_t toTime(timeval value);

/// Converts std::time_t value to std::timespec.
/// @param value            Value to be converted.
/// @return std::timespec created from std::time_t.
timespec toTimespec(std::time_t value);

/// Converts std::tm value to std::timespec.
/// @param value            Value to be converted.
/// @return std::timespec created from std::tm.
timespec toTimespec(std::tm value);

/// Converts std::timeval value to std::timespec.
/// @param value            Value to be converted.
/// @return std::timespec created from std::timeval.
timespec toTimespec(timeval value);

/// Converts std::time_t value to std::timeval.
/// @param value            Value to be converted.
/// @return std::timeval created from std::time_t.
timeval toTimeval(std::time_t value);

/// Converts std::tm value to std::timeval.
/// @param value            Value to be converted.
/// @return std::timeval created from std::tm.
timeval toTimeval(std::tm value);

/// Converts std::timespec value to std::timeval.
/// @param value            Value to be converted.
/// @return std::timeval created from std::timespec.
timeval toTimeval(timespec value);

/// Converts std::tm to a string representation using given string format.
/// @param value            Value to be converted to string.
/// @param format           Date/time format to be used.
/// @return String representation of std::tm.
std::string toString(std::tm value, OsalTimeStringFormat format = OsalTimeStringFormat::eTimeDate);

/// Converts std::time_t to a string representation using given string format.
/// @param value            Value to be converted to string.
/// @param format           Date/time format to be used.
/// @return String representation of std::time_t.
std::string toString(std::time_t value, OsalTimeStringFormat format = OsalTimeStringFormat::eTimeDate);

/// Converts std::timespec to a string representation using given string format.
/// @param value            Value to be converted to string.
/// @param format           Date/time format to be used.
/// @return String representation of std::timespec.
std::string toString(timespec value, OsalTimeStringFormat format = OsalTimeStringFormat::eTimeDate);

/// Converts std::timeval to a string representation using given string format.
/// @param value            Value to be converted to string.
/// @param format           Date/time format to be used.
/// @return String representation of std::timeval.
std::string toString(timeval value, OsalTimeStringFormat format = OsalTimeStringFormat::eTimeDate);

} // namespace osal
