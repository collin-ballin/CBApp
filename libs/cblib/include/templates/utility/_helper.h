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

//  "dependent_false_v"
//      Fallback for dependent-false
//
template<class>
inline constexpr bool               dependent_false_v       = false;


//  "enum_count_v"
//
template<class E>
inline constexpr std::size_t        enum_count_v            = [](void) consteval
{
    static_assert(std::is_enum_v<E>, "E must be an enum type");
    if constexpr (requires { E::Count; })
        return static_cast<std::size_t>(E::Count);
    else if constexpr (requires { E::COUNT; })
        return static_cast<std::size_t>(E::COUNT);
    else
        static_assert(dependent_false_v<E>, "Enum must define Count or COUNT");
} (   );



//  "EnumArray"
//      Simple Struct/Class to use Indices based off Enum Class-Members Specifically (no need for static casting).
//
template<typename E, typename T, std::size_t N = enum_count_v<E>>
requires std::is_enum_v<E>
struct EnumArray {
    std::array<T, N> data{};
    T&       operator[](E e)       noexcept { return data[static_cast<std::size_t>(e)]; }
    const T& operator[](E e) const noexcept { return data[static_cast<std::size_t>(e)]; }
};

/*template<typename E, typename T, std::size_t N = static_cast<std::size_t>(E::Count)>    //  We may need to change from (E::Count) to (E::COUNT)  //
struct EnumArray {
    std::array<T, N>        data;
    T &                     operator []         (E e) noexcept              { return data[static_cast<std::size_t>(e)]; }
    const T &               operator []         (E e) const noexcept        { return data[static_cast<std::size_t>(e)]; }
};*/

//  "enum_index_t"
//
template <typename E>
using enum_index_t = std::underlying_type_t<E>;





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
#endif  //  _CBLIB_HELPER_H  //
