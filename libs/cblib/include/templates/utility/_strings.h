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


//      "join_ptr"
//          - NTTPs are pointers-to-pointer
//
//  template<const char * const * ... ParPtrs>
//  struct join_ptr
//  {
//      static constexpr std::size_t len = (cc_strlen_IMPL(*ParPtrs) + ... + 0);
//
//      static constexpr auto impl() {
//          std::array<char, len + 1> buf{};
//          std::size_t pos = 0;
//          (([&]{
//              const char* str = *ParPtrs;
//              for (std::size_t i = 0, n = cc_strlen_IMPL(str); i < n; ++i)
//                  buf[pos++] = str[i];
//          }()), ...);
//          buf[len] = '\0';
//          return buf;
//      }
//
//      static constexpr auto arr   = impl();
//      static constexpr const char* c_str = arr.data();
//  };
//
//
//  //  "strcat_constexpr"
//  //  Define "aliases" for convience...
//  //
//  template<const char * const * ... ParPtrs>
//  inline constexpr const char * strcat_constexpr = join_ptr<ParPtrs...>::c_str;






// *************************************************************************** //
//
//
//
//  3.  COMPILE-TIME CONST CHAR * CONCATONATION...
// *************************************************************************** //
// *************************************************************************** //

//  "strcat_string_view_cx"
//
//  "_CX"    =   "constexpr"        :  "COMPILE-TIME OPERATION" ...
//
template <std::string_view const&... Strs>
struct strcat_string_view_cx {
    static constexpr auto impl() noexcept {
        constexpr std::size_t           len     = (Strs.size() + ... + 0);
        std::array<char, len + 1>       word_   {};
        
        auto append = [i = 0, &word_](auto const& s) mutable {
            for (char c : s) word_[i++] = c;
        };
        (append(Strs), ...);
        word_[len] = '\0';
        return word_;
    }

    static constexpr auto               arr                 = impl();
    static constexpr std::string_view   value               { arr.data(), arr.size() - 1 };
    static constexpr const char *       c_str               = arr.data();
};



template <std::string_view const&... Strs>
inline constexpr auto                   strcat_cx_v         = strcat_string_view_cx<Strs...>::value;

template <std::string_view const&... Strs>
inline constexpr const char *           strcat_cx_cstr      = strcat_string_view_cx<Strs...>::c_str;






// *************************************************************************** //
//
//
//
//      4.      STRING UTILITY FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "fmt_file_size"
//
[[nodiscard]] inline std::string    fmt_file_size               (const std::uintmax_t bytes) {
    static constexpr const char *   units[]         = { "B", "KB", "MB", "GB", "TB" };
    static constexpr size_t         SIZE            = 64;
    //
    char                            buf[SIZE]       = {};
    double                          value           = static_cast<double>(bytes);
    int                             idx             = 0;
    
    while ( value >= 1024.0 && idx < 4 )            { value /= 1024.0; ++idx; }
    std::snprintf(buf, sizeof(buf), (idx == 0) ? "%.0f %s" : "%.1f %s", value, units[idx]);
    return buf;
};
    
    
//  "fmt_imgui_string"
//
[[nodiscard]] inline static std::string fmt_imgui_string(const char * c_string, const std::size_t N = 64)
{
    using       sv                          = std::string_view;
    constexpr   sv          pair            = "##";
    constexpr   sv          ellipsis        = "...";
    const       sv          s               = (c_string)    ? sv{c_string}  : sv{  };       // null-safe
    sv                      core;
    std::string             out;
    const std::size_t       pos             = s.find(pair);                                 // --- normalize by first occurrence of "##"
    const std::size_t       keep            = N - ellipsis.size();
    
    
    if ( pos == sv::npos )                  { core = s;                     }       //  (1) absent
    else if ( pos == 0 )                    { core = s.substr(2);           }       //  (2) leading
    else                                    { core = s.substr(0, pos);      }       //  (3)/(4) before pair


    // --- truncate with ellipsis policy
    if ( core.size() <= N )                 { return std::string{core}; }

    if ( N < ellipsis.size() )              { return std::string{core.substr(0, N)}; }   // Not enough room for full ellipsis: return raw prefix

    out.reserve( N );
    out.append( core.substr(0, keep) );
    out.append( ellipsis );
    
    
    return out; // length <= N and == N when N>0
}
    









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "utl" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_UTILITY_STRINGS_H  //
