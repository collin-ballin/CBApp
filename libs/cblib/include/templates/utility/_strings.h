/***********************************************************************************
*
*       ********************************************************************
*       ****             _ S T R I N G S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 07, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_UTILITY_STRINGS_H
#define _CBLIB_UTILITY_STRINGS_H 1

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdio.h>
#include <stdexcept>    // std::runtime_error

#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <string>
#include <string_view>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //


namespace cblib { namespace utl {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERAL STRING OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "get_timestamp"
//
[[nodiscard]] inline std::chrono::time_point<std::chrono::system_clock> get_timestamp(void) {
    return std::chrono::system_clock::now();
}

//  "timestamp_to_str"
//
[[nodiscard]] inline std::string timestamp_to_str(std::chrono::time_point<std::chrono::system_clock> & now, const char * fmt = "%Y-%m-%dT%H:%M:%S") {
    std::time_t             t_c     = std::chrono::system_clock::to_time_t(now);
    std::tm                 tm;
    std::ostringstream      oss;
#if defined(_WIN32)
    localtime_s(&tm, &t_c);
# else
    localtime_r(&t_c, &tm);
# endif //  _WIN32  //
    oss << std::put_time(&tm, fmt);
    return oss.str();
}


//  "get_timestamp_str"
//
[[nodiscard]] inline std::string get_timestamp_str(const char * fmt = "%Y-%m-%dT%H:%M:%S") {
    auto                    now     = std::chrono::system_clock::now();
    std::time_t             t_c     = std::chrono::system_clock::to_time_t(now);
    std::tm                 tm;
    std::ostringstream      oss;
#if defined(_WIN32)
    localtime_s(&tm, &t_c);
# else
    localtime_r(&t_c, &tm);
# endif //  _WIN32  //
    oss << std::put_time(&tm, fmt);
    return oss.str();
}


//  "string_to_timestamp"
//
/**
 * Parse a timestamp string back into a system_clock time_point.
 *
 * @param ts_str   The timestamp string, e.g. "2025-06-07T17:30:15"
 * @param fmt      A strftime‐style format (default matches get_timestamp's "%Y-%m-%dT%H:%M:%S")
 * @return         std::chrono::system_clock::time_point corresponding to the local time parsed.
 * @throws         std::runtime_error if parsing or conversion fails.
 */
[[nodiscard]] inline std::chrono::system_clock::time_point
string_to_timestamp(const std::string & ts_str, const char * fmt = "%Y-%m-%dT%H:%M:%S")
{
    std::tm tm{};  // zero-initialized
    std::istringstream iss(ts_str);
    iss >> std::get_time(&tm, fmt);
    if (iss.fail()) {
        throw std::runtime_error("string_to_timestamp: failed to parse time string");
    }

    // mktime treats 'tm' as local time and returns time_t
    std::time_t tt = std::mktime(&tm);
    if (tt == -1) {
        throw std::runtime_error("string_to_timestamp: mktime conversion failed");
    }

    return std::chrono::system_clock::from_time_t(tt);
}






//  "format_elapsed_timestamp"
//
inline std::string format_elapsed_timestamp(std::chrono::system_clock::duration dt) 
{
    using clock             = std::chrono::system_clock                     ;   using duration      = clock::duration;
    using days              = std::chrono::days                             ;   using hours         = std::chrono::hours;
    using minutes           = std::chrono::minutes                          ;   using seconds       = std::chrono::seconds;
    using milliseconds      = std::chrono::milliseconds                     ;   using microseconds  = std::chrono::microseconds;

    // 1) peel off each component
    std::ostringstream  oss;
    days                d   = std::chrono::duration_cast<days>(dt)          ;   dt -= d;
    hours               h   = std::chrono::duration_cast<hours>(dt)         ;   dt -= h;
    minutes             m   = std::chrono::duration_cast<minutes>(dt)       ;   dt -= m;
    seconds             s   = std::chrono::duration_cast<seconds>(dt)       ;   dt -= s;
    milliseconds        ms  = std::chrono::duration_cast<milliseconds>(dt)  ;   dt -= ms;
    microseconds        us  = std::chrono::duration_cast<microseconds>(dt)  ;
    oss << std::setfill('0');

    if (d.count() > 0) {
        // 2+ days: “Nd HH:MM:SS”
        oss << d.count() << "days "
            << std::setw(2) << h.count() << "hours : "
            << std::setw(2) << m.count() << "mins  : "
            << std::setw(2) << s.count() << "secs";
    }
    else if (h.count() > 0) {
        // 1+ hours: “HH:MM:SS”
        oss << std::setw(2) << h.count() << "hours : "
            << std::setw(2) << m.count() << "mins  : "
            << std::setw(2) << s.count() << "secs";
    }
    else if (m.count() > 0) {
        // 1+ minutes: “MM:SS.mmm”
        oss << std::setw(2) << m.count() << "mins  : "
            << std::setw(2) << s.count() << "s."
            << std::setw(3) << ms.count()   << "ms";
    }
    else if (dt < std::chrono::seconds(1)) {
        // < 1 second: use native chrono output (e.g. "123456µs")
        std::ostringstream tmp;
        tmp << dt;
        oss << tmp.str();
    }
    else {
        // ≥ 1 second (but < 1 minute): “SS.mmm”
        auto secs   = std::chrono::duration_cast<std::chrono::seconds>(dt).count();
        auto ms_rem = std::chrono::duration_cast<std::chrono::milliseconds>(dt - std::chrono::seconds(secs)).count();
        oss << std::setw(2) << secs
            << "." << std::setw(3) << ms_rem;
    }
    
    
    

    return oss.str();
}






// *************************************************************************** //
//
//
//  2.  COMPILE-TIME STRING STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  cc_strlen_IMPL
//
constexpr std::size_t cc_strlen_IMPL(const char * s) noexcept {
    std::size_t     i = 0;
    while (s[i] != '\0') ++i;
    return i;
}


//  "join_ptr"
//      - NTTPs are pointers-to-pointer
//
template<const char * const * ... ParPtrs>
struct join_ptr
{
    static constexpr std::size_t len = (cc_strlen_IMPL(*ParPtrs) + ... + 0);

    static constexpr auto impl() {
        std::array<char, len + 1> buf{};
        std::size_t pos = 0;
        (([&]{
            const char* str = *ParPtrs;
            for (std::size_t i = 0, n = cc_strlen_IMPL(str); i < n; ++i)
                buf[pos++] = str[i];
        }()), ...);
        buf[len] = '\0';
        return buf;
    }

    static constexpr auto arr   = impl();
    static constexpr const char* c_str = arr.data();
};


//  "strcat_constexpr"
//  Define "aliases" for convience...
//
template<const char * const * ... ParPtrs>
inline constexpr const char * strcat_constexpr = join_ptr<ParPtrs...>::c_str;











// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_UTILITY_STRINGS_H  //
