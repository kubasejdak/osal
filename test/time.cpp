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

#include <osal/time.h> // NOLINT(modernize-deprecated-headers,hicpp-deprecated-headers)
#include <osal/time.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <array>
#include <string>

static constexpr int cDatesCount = 5;     // NOLINT
static constexpr int cDate1 = 1643407982; // NOLINT
static constexpr int cDate2 = 871571145;  // NOLINT
static constexpr int cDate3 = 1009832777; // NOLINT
static constexpr int cDate4 = 21902400;   // NOLINT
static constexpr int cDate5 = 650146800;  // NOLINT

TEST_CASE("Convert to struct tm", "[unit][c][time]")
{
    std::array<std::tm, cDatesCount> tms{};

    SECTION("time_t to struct tm")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        std::transform(times.begin(), times.end(), tms.begin(), osalTimeToTm);
    }

    SECTION("struct timespec to struct tm")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timespecs.begin(), timespecs.end(), tms.begin(), osalTimespecToTm);
    }

    SECTION("struct timeval to struct tm")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timevals.begin(), timevals.end(), tms.begin(), osalTimevalToTm);
    }

    REQUIRE(tms[0].tm_mday == 28);
    REQUIRE(tms[0].tm_mon == 0);
    REQUIRE(tms[0].tm_year == 122);
    REQUIRE(tms[0].tm_hour == 22);
    REQUIRE(tms[0].tm_min == 13);
    REQUIRE(tms[0].tm_sec == 2);
    REQUIRE(tms[0].tm_wday == 5);
    REQUIRE(tms[0].tm_yday == 27);

    REQUIRE(tms[1].tm_mday == 14);
    REQUIRE(tms[1].tm_mon == 7);
    REQUIRE(tms[1].tm_year == 97);
    REQUIRE(tms[1].tm_hour == 15);
    REQUIRE(tms[1].tm_min == 5);
    REQUIRE(tms[1].tm_sec == 45);
    REQUIRE(tms[1].tm_wday == 4);
    REQUIRE(tms[1].tm_yday == 225);

    REQUIRE(tms[2].tm_mday == 31);
    REQUIRE(tms[2].tm_mon == 11);
    REQUIRE(tms[2].tm_year == 101);
    REQUIRE(tms[2].tm_hour == 21);
    REQUIRE(tms[2].tm_min == 6);
    REQUIRE(tms[2].tm_sec == 17);
    REQUIRE(tms[2].tm_wday == 1);
    REQUIRE(tms[2].tm_yday == 364);

    REQUIRE(tms[3].tm_mday == 11);
    REQUIRE(tms[3].tm_mon == 8);
    REQUIRE(tms[3].tm_year == 70);
    REQUIRE(tms[3].tm_hour == 12);
    REQUIRE(tms[3].tm_min == 0);
    REQUIRE(tms[3].tm_sec == 0);
    REQUIRE(tms[3].tm_wday == 5);
    REQUIRE(tms[3].tm_yday == 253);

    REQUIRE(tms[4].tm_mday == 8);
    REQUIRE(tms[4].tm_mon == 7);
    REQUIRE(tms[4].tm_year == 90);
    REQUIRE(tms[4].tm_hour == 20);
    REQUIRE(tms[4].tm_min == 20);
    REQUIRE(tms[4].tm_sec == 0);
    REQUIRE(tms[4].tm_wday == 3);
    REQUIRE(tms[4].tm_yday == 219);
}

TEST_CASE("Convert to time_t", "[unit][c][time]")
{
    std::array<std::time_t, cDatesCount> times{};

    SECTION("struct tm to time_t")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        std::transform(tms.begin(), tms.end(), times.begin(), osalTmToTime);
    }

    SECTION("struct timespec to time_t")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timespecs.begin(), timespecs.end(), times.begin(), osalTimespecToTime);
    }

    SECTION("struct timeval to time_t")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timevals.begin(), timevals.end(), times.begin(), osalTimevalToTime);
    }

    REQUIRE(times[0] == cDate1);
    REQUIRE(times[1] == cDate2);
    REQUIRE(times[2] == cDate3);
    REQUIRE(times[3] == cDate4);
    REQUIRE(times[4] == cDate5);
}

TEST_CASE("Convert to struct timespec", "[unit][c][time]")
{
    std::array<timespec, cDatesCount> timespecs{};

    SECTION("struct tm to struct timespec")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        std::transform(tms.begin(), tms.end(), timespecs.begin(), osalTmToTimespec);
    }

    SECTION("time_t to struct timespec")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        std::transform(times.begin(), times.end(), timespecs.begin(), osalTimeToTimespec);
    }

    SECTION("struct timeval to time_t")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timevals.begin(), timevals.end(), timespecs.begin(), osalTimevalToTimespec);
    }

    REQUIRE(timespecs[0].tv_sec == cDate1);
    REQUIRE(timespecs[0].tv_nsec == 0);
    REQUIRE(timespecs[1].tv_sec == cDate2);
    REQUIRE(timespecs[1].tv_nsec == 0);
    REQUIRE(timespecs[2].tv_sec == cDate3);
    REQUIRE(timespecs[2].tv_nsec == 0);
    REQUIRE(timespecs[3].tv_sec == cDate4);
    REQUIRE(timespecs[3].tv_nsec == 0);
    REQUIRE(timespecs[4].tv_sec == cDate5);
    REQUIRE(timespecs[4].tv_nsec == 0);
}

TEST_CASE("Convert to struct timeval", "[unit][c][time]")
{
    std::array<timeval, cDatesCount> timevals{};

    SECTION("struct tm to struct timeval")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        std::transform(tms.begin(), tms.end(), timevals.begin(), osalTmToTimeval);
    }

    SECTION("time_t to struct timeval")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        std::transform(times.begin(), times.end(), timevals.begin(), osalTimeToTimeval);
    }

    SECTION("struct timespec to struct timeval")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        std::transform(timespecs.begin(), timespecs.end(), timevals.begin(), osalTimespecToTimeval);
    }

    REQUIRE(timevals[0].tv_sec == cDate1);
    REQUIRE(timevals[0].tv_usec == 0);
    REQUIRE(timevals[1].tv_sec == cDate2);
    REQUIRE(timevals[1].tv_usec == 0);
    REQUIRE(timevals[2].tv_sec == cDate3);
    REQUIRE(timevals[2].tv_usec == 0);
    REQUIRE(timevals[3].tv_sec == cDate4);
    REQUIRE(timevals[3].tv_usec == 0);
    REQUIRE(timevals[4].tv_sec == cDate5);
    REQUIRE(timevals[4].tv_usec == 0);
}

TEST_CASE("Convert to string", "[unit][c][time]")
{
    constexpr std::size_t cSize = 20;
    std::array<std::array<char, cSize>, cDatesCount> time{};
    std::array<std::array<char, cSize>, cDatesCount> date{};
    std::array<std::array<char, cSize>, cDatesCount> timeDate{};
    std::array<std::array<char, cSize>, cDatesCount> sortedDateTime{};

    SECTION("struct tm to string")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        for (std::size_t i = 0; i < tms.size(); ++i) {
            auto error = osalTmToString(tms[i], time[i].data(), cSize, OsalTimeStringFormat::eTime);
            REQUIRE(error == OsalError::eOk);

            error = osalTmToString(tms[i], date[i].data(), cSize, OsalTimeStringFormat::eDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTmToString(tms[i], timeDate[i].data(), cSize, OsalTimeStringFormat::eTimeDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTmToString(tms[i], sortedDateTime[i].data(), cSize, OsalTimeStringFormat::eSortedDateTime);
            REQUIRE(error == OsalError::eOk);
        }
    }

    SECTION("time_t to string")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        for (std::size_t i = 0; i < times.size(); ++i) {
            auto error = osalTimeToString(times[i], time[i].data(), cSize, OsalTimeStringFormat::eTime);
            REQUIRE(error == OsalError::eOk);

            error = osalTimeToString(times[i], date[i].data(), cSize, OsalTimeStringFormat::eDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimeToString(times[i], timeDate[i].data(), cSize, OsalTimeStringFormat::eTimeDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimeToString(times[i], sortedDateTime[i].data(), cSize, OsalTimeStringFormat::eSortedDateTime);
            REQUIRE(error == OsalError::eOk);
        }
    }

    SECTION("struct timespec to string")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timespecs.size(); ++i) {
            auto error = osalTimespecToString(timespecs[i], time[i].data(), cSize, OsalTimeStringFormat::eTime);
            REQUIRE(error == OsalError::eOk);

            error = osalTimespecToString(timespecs[i], date[i].data(), cSize, OsalTimeStringFormat::eDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimespecToString(timespecs[i], timeDate[i].data(), cSize, OsalTimeStringFormat::eTimeDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimespecToString(timespecs[i],
                                         sortedDateTime[i].data(),
                                         cSize,
                                         OsalTimeStringFormat::eSortedDateTime);
            REQUIRE(error == OsalError::eOk);
        }
    }

    SECTION("struct timeval to string")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timevals.size(); ++i) {
            auto error = osalTimevalToString(timevals[i], time[i].data(), cSize, OsalTimeStringFormat::eTime);
            REQUIRE(error == OsalError::eOk);

            error = osalTimevalToString(timevals[i], date[i].data(), cSize, OsalTimeStringFormat::eDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimevalToString(timevals[i], timeDate[i].data(), cSize, OsalTimeStringFormat::eTimeDate);
            REQUIRE(error == OsalError::eOk);

            error = osalTimevalToString(timevals[i],
                                        sortedDateTime[i].data(),
                                        cSize,
                                        OsalTimeStringFormat::eSortedDateTime);
            REQUIRE(error == OsalError::eOk);
        }
    }

    REQUIRE(std::string_view{time[0].data()} == "22:13:02");
    REQUIRE(std::string_view{date[0].data()} == "28.01.2022");
    REQUIRE(std::string_view{timeDate[0].data()} == "22:13:02 28.01.2022");
    REQUIRE(std::string_view{sortedDateTime[0].data()} == "20220128_221302");

    REQUIRE(std::string_view{time[1].data()} == "15:05:45");
    REQUIRE(std::string_view{date[1].data()} == "14.08.1997");
    REQUIRE(std::string_view{timeDate[1].data()} == "15:05:45 14.08.1997");
    REQUIRE(std::string_view{sortedDateTime[1].data()} == "19970814_150545");

    REQUIRE(std::string_view{time[2].data()} == "21:06:17");
    REQUIRE(std::string_view{date[2].data()} == "31.12.2001");
    REQUIRE(std::string_view{timeDate[2].data()} == "21:06:17 31.12.2001");
    REQUIRE(std::string_view{sortedDateTime[2].data()} == "20011231_210617");

    REQUIRE(std::string_view{time[3].data()} == "12:00:00");
    REQUIRE(std::string_view{date[3].data()} == "11.09.1970");
    REQUIRE(std::string_view{timeDate[3].data()} == "12:00:00 11.09.1970");
    REQUIRE(std::string_view{sortedDateTime[3].data()} == "19700911_120000");

    REQUIRE(std::string_view{time[4].data()} == "20:20:00");
    REQUIRE(std::string_view{date[4].data()} == "08.08.1990");
    REQUIRE(std::string_view{timeDate[4].data()} == "20:20:00 08.08.1990");
    REQUIRE(std::string_view{sortedDateTime[4].data()} == "19900808_202000");
}

TEST_CASE("Convert to std::tm in C++", "[unit][cpp][time]")
{
    std::array<std::tm, cDatesCount> tms{};

    SECTION("std::time_t to std::tm")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        for (std::size_t i = 0; i < times.size(); ++i)
            tms[i] = osal::toTm(times[i]);
    }

    SECTION("struct timespec to struct tm")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timespecs.size(); ++i)
            tms[i] = osal::toTm(timespecs[i]);
    }

    SECTION("struct timeval to struct tm")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timevals.size(); ++i)
            tms[i] = osal::toTm(timevals[i]);
    }

    REQUIRE(tms[0].tm_mday == 28);
    REQUIRE(tms[0].tm_mon == 0);
    REQUIRE(tms[0].tm_year == 122);
    REQUIRE(tms[0].tm_hour == 22);
    REQUIRE(tms[0].tm_min == 13);
    REQUIRE(tms[0].tm_sec == 2);
    REQUIRE(tms[0].tm_wday == 5);
    REQUIRE(tms[0].tm_yday == 27);

    REQUIRE(tms[1].tm_mday == 14);
    REQUIRE(tms[1].tm_mon == 7);
    REQUIRE(tms[1].tm_year == 97);
    REQUIRE(tms[1].tm_hour == 15);
    REQUIRE(tms[1].tm_min == 5);
    REQUIRE(tms[1].tm_sec == 45);
    REQUIRE(tms[1].tm_wday == 4);
    REQUIRE(tms[1].tm_yday == 225);

    REQUIRE(tms[2].tm_mday == 31);
    REQUIRE(tms[2].tm_mon == 11);
    REQUIRE(tms[2].tm_year == 101);
    REQUIRE(tms[2].tm_hour == 21);
    REQUIRE(tms[2].tm_min == 6);
    REQUIRE(tms[2].tm_sec == 17);
    REQUIRE(tms[2].tm_wday == 1);
    REQUIRE(tms[2].tm_yday == 364);

    REQUIRE(tms[3].tm_mday == 11);
    REQUIRE(tms[3].tm_mon == 8);
    REQUIRE(tms[3].tm_year == 70);
    REQUIRE(tms[3].tm_hour == 12);
    REQUIRE(tms[3].tm_min == 0);
    REQUIRE(tms[3].tm_sec == 0);
    REQUIRE(tms[3].tm_wday == 5);
    REQUIRE(tms[3].tm_yday == 253);

    REQUIRE(tms[4].tm_mday == 8);
    REQUIRE(tms[4].tm_mon == 7);
    REQUIRE(tms[4].tm_year == 90);
    REQUIRE(tms[4].tm_hour == 20);
    REQUIRE(tms[4].tm_min == 20);
    REQUIRE(tms[4].tm_sec == 0);
    REQUIRE(tms[4].tm_wday == 3);
    REQUIRE(tms[4].tm_yday == 219);
}

TEST_CASE("Convert to std::time_t in C++", "[unit][cpp][time]")
{
    std::array<std::time_t, cDatesCount> times{};

    SECTION("std::tm to std::time_t")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        for (std::size_t i = 0; i < tms.size(); ++i)
            times[i] = osal::toTime(tms[i]);
    }

    SECTION("std::timespec to std::time_t")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timespecs.size(); ++i)
            times[i] = osal::toTime(timespecs[i]);
    }

    SECTION("std::timeval to std::time_t")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timevals.size(); ++i)
            times[i] = osal::toTime(timevals[i]);
    }

    REQUIRE(times[0] == cDate1);
    REQUIRE(times[1] == cDate2);
    REQUIRE(times[2] == cDate3);
    REQUIRE(times[3] == cDate4);
    REQUIRE(times[4] == cDate5);
}

TEST_CASE("Convert to std::timespec in C++", "[unit][cpp][time]")
{
    std::array<timespec, cDatesCount> timespecs{};

    SECTION("std::tm to std::timespec")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        for (std::size_t i = 0; i < tms.size(); ++i)
            timespecs[i] = osal::toTimespec(tms[i]);
    }

    SECTION("std::time_t to std::timespec")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        for (std::size_t i = 0; i < times.size(); ++i)
            timespecs[i] = osal::toTimespec(times[i]);
    }

    SECTION("std::timeval to std::time_t")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timevals.size(); ++i)
            timespecs[i] = osal::toTimespec(timevals[i]);
    }

    REQUIRE(timespecs[0].tv_sec == cDate1);
    REQUIRE(timespecs[0].tv_nsec == 0);
    REQUIRE(timespecs[1].tv_sec == cDate2);
    REQUIRE(timespecs[1].tv_nsec == 0);
    REQUIRE(timespecs[2].tv_sec == cDate3);
    REQUIRE(timespecs[2].tv_nsec == 0);
    REQUIRE(timespecs[3].tv_sec == cDate4);
    REQUIRE(timespecs[3].tv_nsec == 0);
    REQUIRE(timespecs[4].tv_sec == cDate5);
    REQUIRE(timespecs[4].tv_nsec == 0);
}

TEST_CASE("Convert to std::timeval in C++", "[unit][cpp][time]")
{
    std::array<timeval, cDatesCount> timevals{};

    SECTION("std::tm to std::timeval")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        for (std::size_t i = 0; i < tms.size(); ++i)
            timevals[i] = osal::toTimeval(tms[i]);
    }

    SECTION("std::time_t to std::timeval")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        for (std::size_t i = 0; i < times.size(); ++i)
            timevals[i] = osal::toTimeval(times[i]);
    }

    SECTION("std::timespec to std::timeval")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timespecs.size(); ++i)
            timevals[i] = osal::toTimeval(timespecs[i]);
    }

    REQUIRE(timevals[0].tv_sec == cDate1);
    REQUIRE(timevals[0].tv_usec == 0);
    REQUIRE(timevals[1].tv_sec == cDate2);
    REQUIRE(timevals[1].tv_usec == 0);
    REQUIRE(timevals[2].tv_sec == cDate3);
    REQUIRE(timevals[2].tv_usec == 0);
    REQUIRE(timevals[3].tv_sec == cDate4);
    REQUIRE(timevals[3].tv_usec == 0);
    REQUIRE(timevals[4].tv_sec == cDate5);
    REQUIRE(timevals[4].tv_usec == 0);
}

TEST_CASE("Convert to std::string in C++", "[unit][cpp][time]")
{
    std::array<std::string, cDatesCount> time{};
    std::array<std::string, cDatesCount> date{};
    std::array<std::string, cDatesCount> timeDate{};
    std::array<std::string, cDatesCount> sortedDateTime{};

    SECTION("std::tm to std::string")
    {
        std::array<std::tm, cDatesCount> tms{};
        tms[0].tm_mday = 28;  // NOLINT
        tms[0].tm_mon = 0;    // NOLINT
        tms[0].tm_year = 122; // NOLINT
        tms[0].tm_hour = 22;  // NOLINT
        tms[0].tm_min = 13;   // NOLINT
        tms[0].tm_sec = 2;    // NOLINT
        tms[0].tm_wday = 5;   // NOLINT
        tms[0].tm_yday = 27;  // NOLINT

        tms[1].tm_mday = 14;  // NOLINT
        tms[1].tm_mon = 7;    // NOLINT
        tms[1].tm_year = 97;  // NOLINT
        tms[1].tm_hour = 15;  // NOLINT
        tms[1].tm_min = 5;    // NOLINT
        tms[1].tm_sec = 45;   // NOLINT
        tms[1].tm_wday = 4;   // NOLINT
        tms[1].tm_yday = 225; // NOLINT

        tms[2].tm_mday = 31;  // NOLINT
        tms[2].tm_mon = 11;   // NOLINT
        tms[2].tm_year = 101; // NOLINT
        tms[2].tm_hour = 21;  // NOLINT
        tms[2].tm_min = 6;    // NOLINT
        tms[2].tm_sec = 17;   // NOLINT
        tms[2].tm_wday = 1;   // NOLINT
        tms[2].tm_yday = 364; // NOLINT

        tms[3].tm_mday = 11;  // NOLINT
        tms[3].tm_mon = 8;    // NOLINT
        tms[3].tm_year = 70;  // NOLINT
        tms[3].tm_hour = 12;  // NOLINT
        tms[3].tm_min = 0;    // NOLINT
        tms[3].tm_sec = 0;    // NOLINT
        tms[3].tm_wday = 5;   // NOLINT
        tms[3].tm_yday = 253; // NOLINT

        tms[4].tm_mday = 8;   // NOLINT
        tms[4].tm_mon = 7;    // NOLINT
        tms[4].tm_year = 90;  // NOLINT
        tms[4].tm_hour = 20;  // NOLINT
        tms[4].tm_min = 20;   // NOLINT
        tms[4].tm_sec = 0;    // NOLINT
        tms[4].tm_wday = 3;   // NOLINT
        tms[4].tm_yday = 219; // NOLINT

        for (std::size_t i = 0; i < tms.size(); ++i) {
            time[i] = osal::toString(tms[i], OsalTimeStringFormat::eTime);
            date[i] = osal::toString(tms[i], OsalTimeStringFormat::eDate);
            timeDate[i] = osal::toString(tms[i], OsalTimeStringFormat::eTimeDate);
            sortedDateTime[i] = osal::toString(tms[i], OsalTimeStringFormat::eSortedDateTime);
        }
    }

    SECTION("time_t to string")
    {
        std::array<std::time_t, cDatesCount> times = {cDate1, cDate2, cDate3, cDate4, cDate5};

        for (std::size_t i = 0; i < times.size(); ++i) {
            time[i] = osal::toString(times[i], OsalTimeStringFormat::eTime);
            date[i] = osal::toString(times[i], OsalTimeStringFormat::eDate);
            timeDate[i] = osal::toString(times[i], OsalTimeStringFormat::eTimeDate);
            sortedDateTime[i] = osal::toString(times[i], OsalTimeStringFormat::eSortedDateTime);
        }
    }

    SECTION("struct timespec to string")
    {
        std::array<timespec, cDatesCount> timespecs = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timespecs.size(); ++i) {
            time[i] = osal::toString(timespecs[i], OsalTimeStringFormat::eTime);
            date[i] = osal::toString(timespecs[i], OsalTimeStringFormat::eDate);
            timeDate[i] = osal::toString(timespecs[i], OsalTimeStringFormat::eTimeDate);
            sortedDateTime[i] = osal::toString(timespecs[i], OsalTimeStringFormat::eSortedDateTime);
        }
    }

    SECTION("struct timeval to string")
    {
        std::array<timeval, cDatesCount> timevals = {
            {{cDate1, 0}, {cDate2, 0}, {cDate3, 0}, {cDate4, 0}, {cDate5, 0}}
        };

        for (std::size_t i = 0; i < timevals.size(); ++i) {
            time[i] = osal::toString(timevals[i], OsalTimeStringFormat::eTime);
            date[i] = osal::toString(timevals[i], OsalTimeStringFormat::eDate);
            timeDate[i] = osal::toString(timevals[i], OsalTimeStringFormat::eTimeDate);
            sortedDateTime[i] = osal::toString(timevals[i], OsalTimeStringFormat::eSortedDateTime);
        }
    }

    REQUIRE(time[0] == "22:13:02");
    REQUIRE(date[0] == "28.01.2022");
    REQUIRE(timeDate[0] == "22:13:02 28.01.2022");
    REQUIRE(sortedDateTime[0] == "20220128_221302");

    REQUIRE(time[1] == "15:05:45");
    REQUIRE(date[1] == "14.08.1997");
    REQUIRE(timeDate[1] == "15:05:45 14.08.1997");
    REQUIRE(sortedDateTime[1] == "19970814_150545");

    REQUIRE(time[2] == "21:06:17");
    REQUIRE(date[2] == "31.12.2001");
    REQUIRE(timeDate[2] == "21:06:17 31.12.2001");
    REQUIRE(sortedDateTime[2] == "20011231_210617");

    REQUIRE(time[3] == "12:00:00");
    REQUIRE(date[3] == "11.09.1970");
    REQUIRE(timeDate[3] == "12:00:00 11.09.1970");
    REQUIRE(sortedDateTime[3] == "19700911_120000");

    REQUIRE(time[4] == "20:20:00");
    REQUIRE(date[4] == "08.08.1990");
    REQUIRE(timeDate[4] == "20:20:00 08.08.1990");
    REQUIRE(sortedDateTime[4] == "19900808_202000");
}
