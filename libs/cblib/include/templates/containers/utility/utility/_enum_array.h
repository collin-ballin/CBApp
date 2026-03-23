/***********************************************************************************
*
*       ********************************************************************
*       ****          _ E N U M _ A R R A Y . H  ____  F I L E          ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      November 10, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_CONTAINERS_ENUMARRAY_H
#define _CBLIB_CONTAINERS_ENUMARRAY_H 1

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




// *************************************************************************** //
//
//
//
//      0.      "ANON" INTERNAL NAMESPACE...
// *************************************************************************** //
// *************************************************************************** //
//
namespace anon {   //     BEGIN INTERNAL-HELPER NAMESPACE..
// *************************************************************************** //
// *************************************************************************** //

//  "dependent_false_v"
//      Fallback for dependent-false
//
template<class>
inline constexpr bool               dependent_false_v                               = false;


//  "has_COUNT_v"
//
template <typename E, typename = void>
struct                              has_COUNT                                       : std::false_type
{
};

template <typename E>
struct                              has_COUNT<E, std::void_t<decltype(E::COUNT)>>   : std::true_type
{
};

template <typename E>
inline constexpr bool               has_COUNT_v                                     = has_COUNT<E>::value;


//  "has_Count_v"
//
template <typename E, typename = void>
struct                              has_Count                                       : std::false_type
{
};

template <typename E>
struct                              has_Count<E, std::void_t<decltype(E::Count)>>   : std::true_type
{
};

template <typename E>
inline constexpr bool               has_Count_v                                     = has_Count<E>::value;


//  "enum_array_index_c"
//      Structural requirement for the enum-domain used by "EnumArray".
//
template <typename E>
concept                             enum_array_index_c                              = std::is_enum_v<E> && ( has_COUNT_v<E> || has_Count_v<E> );


//  "enum_count_value_v"
//      Resolve the terminal COUNT / Count sentinel value.
//
template<class E>
inline constexpr E                  enum_count_value_v                              = [](void) consteval
{
    static_assert( std::is_enum_v<E>                , "E must be an enum type"                                                             );
    static_assert( has_COUNT_v<E> || has_Count_v<E> , "template type parameter error: Enum-type \"E\" must define \"E::Count\" or \"E::COUNT\"" );
    
    if constexpr ( has_COUNT_v<E> && has_Count_v<E> ) {
        static_assert( static_cast<size_t>(E::COUNT) == static_cast<size_t>(E::Count)
                     , "template type parameter error: Enum-type \"E\" defines both \"E::COUNT\" and \"E::Count\", but they do not match" );
        return E::COUNT;
    }
    else if constexpr ( has_COUNT_v<E> ) {
        return E::COUNT;
    }
    else if constexpr ( has_Count_v<E> ) {
        return E::Count;
    }
    else {
        static_assert( dependent_false_v<E>
                     , "template type parameter error: Enum-type \"E\" must define \"E::Count\" or \"E::COUNT\"" );
    }
    
    return E{};
} (   );


//  "enum_legacy_count_v"
//      Numeric extent implied by the COUNT / Count sentinel.
//
template<class E>
inline constexpr size_t             enum_legacy_count_v                             = static_cast<size_t>(enum_count_value_v<E>);


//  "enum_values"
//      OPTIONAL CUSTOM-DOMAIN HOOK:
//
//      When specialized for some enum-domain "E", the "values" array defines the
//      usable enumeration values in the storage order used by "EnumArray".  This
//      enables sparse / non-ordinal enums to map into a dense storage extent.
//
//      Example:
//
//          template <>
//          struct enum_values<MySparseEnum>
//          {
//              static constexpr std::array<MySparseEnum, 8>
//                                          values = {{
//                                                MySparseEnum::None
//                                              , MySparseEnum::Debug
//                                              , MySparseEnum::Info
//                                              , MySparseEnum::Warning
//                                              , MySparseEnum::Exception
//                                              , MySparseEnum::Error
//                                              , MySparseEnum::Notify
//                                              , MySparseEnum::Critical
//                                          }};
//          };
//
template <typename E>
struct                              enum_values
{
};


//  "has_enum_values_v"
//
template <typename E, typename = void>
struct                              has_enum_values                                 : std::false_type
{
};

template <typename E>
struct                              has_enum_values<E, std::void_t<decltype(enum_values<E>::values)>> : std::true_type
{
};

template <typename E>
inline constexpr bool               has_enum_values_v                               = has_enum_values<E>::value;


//  "enum_values_are_unique"
//
template <typename E>
consteval bool                      enum_values_are_unique(void)
{
    bool                            result                                          = true;
    
    if constexpr ( has_enum_values_v<E> ) {
        constexpr auto              values                                          = enum_values<E>::values;
        size_t                      i                                               = 0;
        size_t                      j                                               = 0;
        
        for ( i = 0; result && ( i < values.size() ); ++i ) {
            for ( j = ( i + 1 ); result && ( j < values.size() ); ++j ) {
                result = ( values[i] != values[j] );
            }
        }
    }
    
    return result;
}


//  "enum_values_exclude_count_sentinel"
//
template <typename E>
consteval bool                      enum_values_exclude_count_sentinel(void)
{
    bool                            result                                          = true;
    
    if constexpr ( has_enum_values_v<E> ) {
        constexpr auto              values                                          = enum_values<E>::values;
        size_t                      i                                               = 0;
        
        for ( i = 0; result && ( i < values.size() ); ++i ) {
            result = ( values[i] != enum_count_value_v<E> );
        }
    }
    
    return result;
}


//  "enum_count_v"
//      Dense storage count used by "EnumArray".
//
//      LEGACY BEHAVIOR:
//          If no "enum_values<E>::values" specialization exists, then this falls
//          back to the numeric value of "E::COUNT" / "E::Count".
//
//      CUSTOM-DOMAIN BEHAVIOR:
//          If "enum_values<E>::values" exists, then this becomes the number of
//          usable enumeration values listed in that array.
//
template<class E>
inline constexpr size_t             enum_count_v                                    = [](void) consteval
{
    static_assert( enum_array_index_c<E>
                 , "template type parameter error: Enum-type \"E\" must define enumeration value \"E::Count\" or \"E::COUNT\"" );
    
    if constexpr ( has_enum_values_v<E> ) {
        return enum_values<E>::values.size();
    }
    else {
        return enum_legacy_count_v<E>;
    }
} (   );


//  "enum_index_t"
//
template <typename E>
using                               enum_index_t                                    = std::underlying_type_t<E>;


//  "is_valid_enum_value"
//
template <typename E>
    requires enum_array_index_c<E>
constexpr bool                      is_valid_enum_value(E e) noexcept
{
    bool                            result                                          = false;
    
    if constexpr ( has_enum_values_v<E> ) {
        constexpr auto              values                                          = enum_values<E>::values;
        size_t                      i                                               = 0;
        
        for ( i = 0; !result && ( i < values.size() ); ++i ) {
            result = ( values[i] == e );
        }
    }
    else {
        size_t                      index                                           = static_cast<size_t>(e);
        
        result = ( index < enum_legacy_count_v<E> );
    }
    
    return result;
}


//  "to_index"
//
//      LEGACY BEHAVIOR:
//          For ordinal enums, this is equivalent to "static_cast<size_t>(e)".
//
//      CUSTOM-DOMAIN BEHAVIOR:
//          For enums that specialize "enum_values<E>::values", this returns the
//          dense storage index corresponding to the position of "e" in that array.
//
//      NOTE:
//          If the enum-value is invalid / unmapped, this function returns
//          "enum_count_v<E>".
//
template <typename E>
    requires enum_array_index_c<E>
constexpr size_t                    to_index            (E e) noexcept
{
    size_t                          index                                           = enum_count_v<E>;
    
    if constexpr ( has_enum_values_v<E> ) {
        constexpr auto              values                                          = enum_values<E>::values;
        size_t                      i                                               = 0;
        
        for ( i = 0; ( index == enum_count_v<E> ) && ( i < values.size() ); ++i ) {
            if ( values[i] == e )   { index = i; }
        }
    }
    else {
        index = static_cast<size_t>(e);
    }
    
    return index;
}


//  "from_index"
//
//      LEGACY BEHAVIOR:
//          For ordinal enums, this is equivalent to "static_cast<E>(i)".
//
//      CUSTOM-DOMAIN BEHAVIOR:
//          For enums that specialize "enum_values<E>::values", this returns the
//          enum-value stored at dense storage index "i".  If "i" is out of range,
//          the COUNT / Count sentinel is returned.
//
template <typename E>
    requires enum_array_index_c<E>
constexpr E                         from_index          (size_t i) noexcept
{
    E                               value                                           = enum_count_value_v<E>;
    
    if constexpr ( has_enum_values_v<E> ) {
        constexpr auto              values                                          = enum_values<E>::values;
        
        if ( i < values.size() )    { value = values[i]; }
    }
    else {
        value = static_cast<E>(i);
    }
    
    return value;
}


//  "make_filled_array_impl"
//
template <typename T, size_t N, size_t ... I>
constexpr std::array<T, N>          make_filled_array_impl(const T & value, std::index_sequence<I...>)
{
    return std::array<T, N>{ { ( static_cast<void>(I), value ) ... } };
}


//  "make_filled_array"
//
template <typename T, size_t N>
constexpr std::array<T, N>          make_filled_array   (const T & value)
{
    return make_filled_array_impl<T, N>( value, std::make_index_sequence<N>{ } );
}



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "anon" INTERNAL-HELPER NAMESPACE.






// *************************************************************************** //
//
//
//
//      1.      "EnumArray" CLASS IMPLEMENTATION...
// *************************************************************************** //
// *************************************************************************** //

//  "EnumArray"
//      Simple Struct/Class to use Indices based off Enum Class-Members Specifically (no need for static casting).
//
/// @class      EnumArray
///
/// @brief      A thin, enum-indexed wrapper around std::array that provides typed
///             subscripting with support for both legacy ordinal enums and
///             custom-mapped sparse enums.
///
/// @tparam     E           Enum domain type.
/// @tparam     T           Stored value type.
/// @tparam     N           Underlying storage extent. By default this is
///                         anon::enum_count_v<E>.
///
/// @details
///             This abstraction exists to model array-like storage whose indices
///             are drawn from an enumeration domain rather than raw integral
///             values.  The class preserves the original public template shape
///             of the older implementation:
///
///                 EnumArray<E, T, N = anon::enum_count_v<E>>
///
///             while extending the implementation so that it can now support two
///             distinct modes of operation:
///
///             1.  LEGACY / ORDINAL MODE
///                 If no custom domain metadata is provided for E, the class
///                 preserves the legacy interpretation of the enum-domain:
///
///                     - E must define a terminal enumerator named COUNT or Count.
///                     - anon::enum_count_v<E> is the numeric value of that terminal
///                       sentinel.
///                     - anon::to_index(E) falls back to static_cast<size_t>(e).
///
///                 This mode is intended for dense, zero-based ordinal enums of
///                 the form:
///
///                     0, 1, 2, ..., COUNT - 1, COUNT
///
///                 and is preserved primarily for backward compatibility with
///                 older code.
///
///             2.  CUSTOM-DOMAIN MODE
///                 If anon::enum_values<E>::values is specialized, that table
///                 defines the usable enumeration values and their dense storage
///                 order.  In this mode:
///
///                     - anon::enum_count_v<E> becomes enum_values<E>::values.size().
///                     - anon::to_index(E) maps E to the position of E in that table.
///                     - anon::from_index(size_t) maps dense storage indices back
///                       to the corresponding enum value.
///
///                 This mode allows sparse or explicitly-valued enums to be used
///                 correctly with EnumArray while preserving the existing class
///                 template interface.
///
///
/// @par        Problems In The Previous Design
///
/// @details
///             The original implementation contained several important flaws:
///
///             - It sized the underlying std::array solely from the numeric value
///               of E::COUNT / E::Count.  This worked only for dense ordinal enums.
///               Sparse enums such as:
///
///                   None = 0, Debug = 10, Info = 20, ...
///
///               incorrectly produced very large storage extents because COUNT was
///               interpreted numerically rather than semantically.
///
///             - It assumed that enum-to-index conversion was always:
///
///                   static_cast<size_t>(e)
///
///               which is only valid for dense ordinal enums.  Sparse enums require
///               an explicit mapping from enum value to dense storage position.
///
///             - The earlier “zero_based_contiguous” check was not actually a proof
///               of contiguity.  Testing whether E{} converts to zero does not prove
///               that the named enumerators of E form a dense ordinal domain.
///
///             - at(index_type) forwarded the enum directly into std::array::at(...),
///               which is incorrect for scoped enums because std::array::at expects
///               an integral index, not an enum object.
///
///             - The const reverse-iterator overloads returned mutable reverse
///               iterators, which violated const-correctness.
///
///             - The original aggregate-based design caused a single-value brace
///               initializer such as:
///
///                   { T{7} }
///
///               to initialize only the first element and value-initialize the
///               remaining elements.  That behavior was often not what the user
///               intended when supplying a single scalar.
///
///
/// @par        What Was Changed To Correct These Issues
///
/// @details
///             The refactored implementation addresses those problems as follows:
///
///             - COUNT / Count resolution is centralized through internal helper
///               machinery, with support for both legacy spellings preserved.
///
///             - The abstraction now supports optional custom domain metadata via
///               anon::enum_values<E>::values.  When present, that table defines
///               the actual usable enumeration values and their dense storage order.
///
///             - The default storage extent anon::enum_count_v<E> is now:
///
///                   * the size of enum_values<E>::values, if custom metadata exists
///                   * otherwise the legacy numeric COUNT / Count value
///
///               This preserves existing behavior while allowing sparse enums to
///               use the correct logical number of elements.
///
///             - Enum-to-index conversion is now centralized through anon::to_index(...),
///               which either:
///
///                   * performs the legacy raw cast, or
///                   * performs a lookup into the custom enum_values<E>::values table.
///
///             - at(index_type) now performs correct enum-domain validation and
///               throws std::out_of_range for invalid enum-domain indices.
///
///             - Reverse iterator accessors were corrected to provide proper
///               const and non-const overloads.
///
///             - The class now provides a single-value constructor that fills the
///               entire array with that value, enabling:
///
///                   EnumArray<E, T> x = { T{7} };
///
///               to mean “fill all elements with T{7}”.
///
///
/// @par        Important API / Usage Notes
///
/// @details
///             1. COUNT / Count Support
///                The enum-domain E must define a terminal sentinel named either
///                COUNT or Count.  Both forms are supported for backward
///                compatibility with older enum definitions.
///
///             2. Legacy Ordinal Enums
///                For dense ordinal enums, no extra customization is needed.
///                The default behavior remains compatible with the original design.
///
///             3. Sparse / Explicitly-Valued Enums
///                To use sparse enums correctly, specialize:
///
///                    anon::enum_values<E>
///
///                and define:
///
///                    static constexpr std::array<E, M> values = {{ ... }};
///
///                where:
///
///                    - the array lists every usable enumerator exactly once,
///                    - the COUNT / Count sentinel is omitted,
///                    - the order of the array defines the dense storage order.
///
///             4. Checked vs Unchecked Access
///                operator[](E) is an unchecked access path, matching the general
///                spirit of std::array::operator[].
///
///                at(E) is the checked access path.  It validates both the enum
///                value and the mapped storage index before accessing the element.
///
///             5. Single-Value Initialization
///                A single scalar initializer now fills the entire array:
///
///                    EnumArray<E, int> x = { 7 };
///
///                rather than initializing only the first element.
///
///             6. Element-Wise Initialization
///                The variadic constructor still supports explicit N-value
///                initialization when N values are supplied.
///
///             7. Underlying std::array Construction
///                Construction from the underlying std::array type remains
///                supported through the array-taking constructors.
///
///             8. Overriding N
///                The template still permits manual override of N for backward
///                compatibility.  In normal usage, however, N should generally
///                be left at its default so that the storage extent tracks the
///                enum-domain definition.
///
///
/// @warning    Legacy mode still assumes that an enum without custom metadata is
///             intended to behave as an ordinal index domain.  For sparse enums,
///             custom anon::enum_values<E>::values metadata should be provided.
///
/// @warning    The terminal COUNT / Count sentinel is not a usable logical element
///             of the domain and should not be treated as a valid stored entry.
///
/// @note       This abstraction intentionally remains a thin wrapper over
///             std::array.  It does not attempt to behave like an associative
///             container; instead, it formalizes enum-domain indexing while
///             preserving array semantics as closely as practical.
///
/// @see        anon::enum_count_v
/// @see        anon::enum_values
/// @see        anon::to_index
/// @see        anon::from_index
/// @see        EnumArray::at
/// @see        EnumArray::fill
//
template< typename E, typename T, size_t N = anon::enum_count_v<E> >
    requires anon::enum_array_index_c<E>
struct EnumArray
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                                       array_type                      = std::array<T, N>                                  ;
    //
    //                                      VALUE TYPES:
    using                                       value_type                      = typename array_type::value_type                   ;
    using                                       pointer                         = typename array_type::pointer                      ;
    using                                       const_pointer                   = typename array_type::const_pointer                ;
    using                                       reference                       = typename array_type::reference                    ;
    using                                       const_reference                 = typename array_type::const_reference              ;
    //
    //                                      SIZE / INDEX TYPES:
    using                                       index_type                      = E                                                 ;
    using                                       size_type                       = typename array_type::size_type                    ;   //  array_type::size_type;
    using                                       difference_type                 = typename array_type::difference_type              ;
    //
    //                                      ITERATOR TYPES:
    using                                       iterator                        = typename array_type::iterator                     ;
    using                                       const_iterator                  = typename array_type::const_iterator               ;
    using                                       reverse_iterator                = typename array_type::reverse_iterator             ;
    using                                       const_reverse_iterator          = typename array_type::const_reverse_iterator       ;



    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      COMPILE-TIME ENFORCEMENT MECHANISMS.
    // *************************************************************************** //
    static constexpr size_type                   cv_enum_count                   = anon::enum_count_v<E>                             ;
    static constexpr bool                        cv_has_custom_domain            = anon::has_enum_values_v<E>                        ;
    
    static_assert( N > 0
                 , "EnumArray requires N > 0 (empty enum domains are not supported)."                                                 );
    static_assert( !cv_has_custom_domain || anon::enum_values_are_unique<E>( )
                 , "custom enum-domain metadata contains duplicate enumeration values."                                                );
    static_assert( !cv_has_custom_domain || anon::enum_values_exclude_count_sentinel<E>( )
                 , "custom enum-domain metadata must not contain the terminal COUNT / Count sentinel."                                 );
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      DATA MEMBERS.
    // *************************************************************************** //
    array_type                                  m_data                          {   };
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      "RULE-OF ..." FUNCTIONS.
    // *************************************************************************** //
    inline constexpr                            EnumArray           (void)                                          = default;
    
    //  Fill-value CTOR...
    inline constexpr                            EnumArray           (const value_type & fill_value)
        requires std::is_copy_constructible_v<value_type>
        : m_data( anon::make_filled_array<value_type, N>(fill_value) )
    {   }
    
    //  CTOR to accept the underlying array ( enables usage of the form: "DEF_ARRAY = {{ ... }}" )...
    inline constexpr                            EnumArray           (const array_type & a)      : m_data(a)                 {   }
    inline constexpr                            EnumArray           (array_type && a)           : m_data( std::move(a) )    {   }
    
    //  N-value CTOR...
    template <class... U>
        requires ( ( N != 1 ) && ( sizeof...(U) == N ) && ( std::is_constructible_v<value_type, U&&> && ... ) )
    inline constexpr                            EnumArray           (U && ... u)
        : m_data{ { value_type( std::forward<U>(u) ) ... } }
    {   }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OVERLOADED OPERATORS.
    // *************************************************************************** //
    inline constexpr reference                  operator [ ]        (index_type e)       noexcept           { return this->m_data[ anon::to_index(e) ];       }
    inline constexpr const_reference            operator [ ]        (index_type e) const noexcept           { return this->m_data[ anon::to_index(e) ];       }
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STANDARD-LIBRARY COMPLIANCE FUNCTIONS.
    // *************************************************************************** //
    //                                      INFORMATION FUNCTIONS:
    inline constexpr bool                       empty               (void) const noexcept                   { return this->m_data.empty();                    }
    inline constexpr size_type                  size                (void) const noexcept                   { return this->m_data.size();                     }
    inline constexpr size_type                  max_size            (void) const noexcept                   { return this->m_data.max_size();                 }
    //
    //
    //                                      ENUM-DOMAIN INFORMATION:
    inline static constexpr size_type           enum_count          (void) noexcept                         { return cv_enum_count;                           }
    inline static constexpr bool                has_custom_domain   (void) noexcept                         { return cv_has_custom_domain;                    }
    //
    inline static constexpr bool                is_valid_enum_index (index_type idx) noexcept
    {
        size_type                               storage_index                                   = anon::to_index(idx);
        bool                                    result                                          = false;
        
        result = anon::is_valid_enum_value(idx) && ( storage_index < N );
        
        return result;
    }
    //
    inline static constexpr bool                is_valid_storage_index(size_type idx) noexcept             { return ( idx < N );                             }
    //
    //
    //                                      ACCESSOR FUNCTIONS:
    inline reference                            at                  (index_type idx)
    {
        size_type                               storage_index                                   = anon::to_index(idx);
        
        if ( !( anon::is_valid_enum_value(idx) && ( storage_index < N ) ) ) {
            throw std::out_of_range("EnumArray::at(index_type): invalid enum-domain index");
        }
        
        return this->m_data.at(storage_index);
    }
    
    inline const_reference                      at                  (index_type idx) const
    {
        size_type                               storage_index                                   = anon::to_index(idx);
        
        if ( !( anon::is_valid_enum_value(idx) && ( storage_index < N ) ) ) {
            throw std::out_of_range("EnumArray::at(index_type) const: invalid enum-domain index");
        }
        
        return this->m_data.at(storage_index);
    }
    //
    inline constexpr reference                  front               (void) noexcept                         { return this->m_data.front();                    }
    inline constexpr const_reference            front               (void) const noexcept                   { return this->m_data.front();                    }
    inline constexpr reference                  back                (void) noexcept                         { return this->m_data.back();                     }
    inline constexpr const_reference            back                (void) const noexcept                   { return this->m_data.back();                     }
    //
    inline constexpr pointer                    data                (void) noexcept                         { return this->m_data.data();                     }
    inline constexpr const_pointer              data                (void) const noexcept                   { return this->m_data.data();                     }
    //
    //
    //
    //                                      ITERATOR FUNCTIONS:
    inline constexpr iterator                   begin               (void) noexcept                         { return this->m_data.begin();                    }   //  Iterators (enable C++20 ranges/views pipelines)
    inline constexpr const_iterator             begin               (void) const noexcept                   { return this->m_data.begin();                    }
    inline constexpr const_iterator             cbegin              (void) const noexcept                   { return this->m_data.cbegin();                   }
    inline constexpr reverse_iterator           rbegin              (void) noexcept                         { return this->m_data.rbegin();                   }
    inline constexpr const_reverse_iterator     rbegin              (void) const noexcept                   { return this->m_data.rbegin();                   }
    inline constexpr const_reverse_iterator     crbegin             (void) const noexcept                   { return this->m_data.crbegin();                  }
    //
    //
    inline constexpr iterator                   end                 (void) noexcept                         { return this->m_data.end();                      }
    inline constexpr const_iterator             end                 (void) const noexcept                   { return this->m_data.end();                      }
    inline constexpr const_iterator             cend                (void) const noexcept                   { return this->m_data.cend();                     }
    inline constexpr reverse_iterator           rend                (void) noexcept                         { return this->m_data.rend();                     }
    inline constexpr const_reverse_iterator     rend                (void) const noexcept                   { return this->m_data.rend();                     }
    inline constexpr const_reverse_iterator     crend               (void) const noexcept                   { return this->m_data.crend();                    }
    //
    //
    //                                      HELPER FUNCTIONS:
    inline constexpr void                       fill                (const value_type & u)                  { this->m_data.fill(u);                           }
    

//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EnumArray" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "EnumArray" IMPL." ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_CONTAINERS_ENUMARRAY_H  //
