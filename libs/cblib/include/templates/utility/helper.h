#ifndef _CBLIB_HELPER_H
#define _CBLIB_HELPER_H 1

#include <iostream>
#include <type_traits>
#include <array>
#include <cstddef>

#include <vector>
#include <iomanip>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	// C++11.



namespace cblib {   //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //


//  1.  OTHER...
// *************************************************************************** //
// *************************************************************************** //






//  2.  OTHER...
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
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATRIX_DRIVER_H  //
