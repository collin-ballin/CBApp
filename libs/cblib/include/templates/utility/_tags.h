/***********************************************************************************
*
*       ********************************************************************
*       ****                _ T A G S . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 26, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_UTILITY_TAGS_H
#define _CBLIB_UTILITY_TAGS_H 1

#include <iostream>
#include <stdexcept>    // std::runtime_error

#include <type_traits>
#include <algorithm>
# include <concepts>

#include <cstddef>     // size_t
#include <cstdint>     // uint32_t, uint16_t, …
#include <compare>     // <=>
#include <concepts>    // std::integral
#include <functional>  // std::hash



namespace cblib { namespace utl {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //




/// @class      IDType
/// @brief      DESC
///
/// @tparam     T       The underlying integral storage type (e.g. std::uint32_t).
/// @tparam     Tag     An empty struct that uniquely tags the ID category.
///
/// @todo       TODO
///
///

template<std::integral T, class Tag>
class IDType {
// *************************************************************************** //
//      DATA MEMBERS...
// *************************************************************************** //
    T                                   v_                                          = T( );     //  The only data member (zero‑cost)


// *************************************************************************** //
//      PUBLIC API...
// *************************************************************************** //
public:
    using                               underlying_type                             = T;
    //
    //
    //      constructors        //
    constexpr                           IDType          (void) noexcept                 = default;
    constexpr explicit                  IDType          (T v) noexcept  : v_{v}         {  }
    //
    //
    //      accessors           //
    constexpr T                         value           (void) const noexcept           { return v_; }
    //
    //
    //      validity helpers    //
    [[nodiscard]]
    static constexpr IDType             invalid         (void) noexcept                 { return IDType{}; }
    constexpr bool                      valid           (void) const noexcept           { return *this != invalid(); }
    constexpr explicit operator         bool            (void) const noexcept           { return valid(); }
    //
    //
    //      comparison          //
    constexpr friend bool operator      ==              (IDType, IDType) noexcept       = default;
    constexpr friend auto operator      <=>             (IDType, IDType) noexcept       = default;
    //
    //
    //      arithmetic (only if underlying type is unsigned)        //
    template<std::unsigned_integral U = T>
    constexpr IDType &                  operator ++     (void) noexcept                 { ++v_; return *this; }

    template<std::unsigned_integral U = T>
    constexpr IDType                    operator++      (int) noexcept                  { IDType tmp{*this}; ++*this; return tmp; }

    template<std::unsigned_integral U = T>
    constexpr IDType                    next            (void) const noexcept           { return IDType(v_ + 1); }
    //
    //
    //      hash helper         //
    constexpr std::size_t               hash            (void) const noexcept           { return static_cast<std::size_t>(v_); }
 
// *************************************************************************** //
// *************************************************************************** //
};


// *************************************************************************** //
//      CONVENIENCE HELPERS / TEMPLATE TYPE TRAIT HELPERS...
// *************************************************************************** //

template<class Tag, std::integral T>
constexpr T value(IDType<T, Tag> id) noexcept { return id.value(); }

//  Detect whether a type is an IDType

template<typename> struct is_id_type : std::false_type {};

template<std::integral T, class Tag>
struct is_id_type<IDType<T, Tag>> : std::true_type {};

//  Concept: integral OR our ID wrapper

template<typename T>
concept integral_like = std::integral<T> || is_id_type<T>::value;




// convenience free function ---------------------------------------------------
// template<class Tag, std::integral T>
// constexpr T value(IDType<T, Tag> id) noexcept {
//     return id.value();
// }







// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "utl" NAMESPACE.













// ─────────────────────────────────────────────────────────────────────────────
//  std::hash specialisation (kept outside the library namespace)
// ─────────────────────────────────────────────────────────────────────────────
template<std::integral T, class Tag>
struct std::hash<cblib::utl::IDType<T, Tag>> {
    constexpr std::size_t operator()(const cblib::utl::IDType<T, Tag> &id) const noexcept {
        return id.hash();
    }
};


// ostream support (optional) --------------------------------------------------
template<std::integral T, class Tag>
std::ostream &operator<<(std::ostream &os, const cblib::utl::IDType<T, Tag> &id) {
    return os << id.value();
}












// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_UTILITY_TAGS_H  //
