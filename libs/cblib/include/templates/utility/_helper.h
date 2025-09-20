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




// *************************************************************************** //
//
//
//      1.      MISC. HELPERS/UTILITIES...
// *************************************************************************** //
// *************************************************************************** //

namespace anon {   //     BEGINNING ANONYMOUS NAMESPACE..
// *************************************************************************** //
// *************************************************************************** //

//  "dependent_false_v"
//      Fallback for dependent-false
//
template<class>
inline constexpr bool               dependent_false_v                               = false;


//  "enum_count_v"
//
template<class E>
inline constexpr std::size_t        enum_count_v                                    = [](void) consteval
{
    static_assert(std::is_enum_v<E>, "E must be an enum type");
    
    if constexpr      ( requires { E::Count; } )        { return static_cast<std::size_t>(E::Count); }
    else if constexpr ( requires { E::COUNT; } )        { return static_cast<std::size_t>(E::COUNT); }
    else                                                { static_assert(dependent_false_v<E>, "Enum must define Count or COUNT"); }
    return std::size_t{ 0 };
} (   );


//  "enum_index_t"
//
template <typename E>
using                               enum_index_t                                    = std::underlying_type_t<E>;


//  "to_index"
template <typename E>
constexpr std::size_t               to_index            (E e) noexcept              { return static_cast<std::size_t>(e); }

//  "to_index"
template <typename E>
constexpr E                         from_index          (std::size_t i) noexcept    { return static_cast<E>(i); }



// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "anon" ANONYMOUS NAMESPACE.






//  "EnumArray"
//      Simple Struct/Class to use Indices based off Enum Class-Members Specifically (no need for static casting).
//
template< typename E, typename T, std::size_t N = anon::enum_count_v<E> >
    requires std::is_enum_v<E>
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
    //      COMPILE-TIME ENFORCEMENT MECHANISMS.
    // *************************************************************************** //
    static constexpr bool                       zero_based_contiguous           = ( static_cast<size_type>( E{} ) == 0 );
    static_assert( zero_based_contiguous            , "Enum type must begin at 0 and be contiguous up to the final value, COUNT."    );
    static_assert( N > 0                            , "EnumArray requires N > 0 (empty enum domains are not supported)."             );
    
    
    
    // *************************************************************************** //
    //      DATA MEMBERS.
    // *************************************************************************** //
    array_type                                  m_data                          {   };
    
    
    
    // *************************************************************************** //
    //      "RULE-OF ..." FUNCTIONS.
    // *************************************************************************** //
    //  inline constexpr                EnumArray           (void)
    //      requires std::is_default_constructible_v<T>                                 = default;
        
    //  CTOR to accept the underlying array ( enables usage of the form: "DEF_ARRAY = {{ ... }}" )...
    //  inline constexpr explicit       EnumArray           (const array_type & a)      : m_data(a)                 {   }
    //  inline constexpr explicit       EnumArray           (array_type && a)           : m_data( std::move(a) )    {   }
        
    //  N-value CTOR for non-default-constructible "T"...
    //  template <class... U>
    //  inline constexpr                EnumArray           (U && ... u)
    //      requires (sizeof...(U) == N)
    //      : m_data{ { std::forward<U>(u)... } }
    //  {   }
    
    //  template<class... U>
    //      requires ( sizeof...(U) == N) && (std::convertible_to<U, T> && ... )
    //  inline constexpr explicit       EnumArray           (U&&... u)                  : m_data{ { static_cast<T>(std::forward<U>(u))... } } {   }
    
    
    
    // *************************************************************************** //
    //      OVERLOADED OPERATORS.
    // *************************************************************************** //
    inline constexpr reference                  operator [ ]        (index_type e)       noexcept           { return m_data[ static_cast<size_type>(e) ];     }
    inline constexpr const_reference            operator [ ]        (index_type e) const noexcept           { return m_data[ static_cast<size_type>(e) ];     }
    
    
    // *************************************************************************** //
    //      STANDARD-LIBRARY COMPLIANCE FUNCTIONS.
    // *************************************************************************** //
    //                                      INFORMATION FUNCTIONS:
    inline size_type                            size                (void) const noexcept                   { return this->m_data.size();       }
    inline constexpr bool                       max_size            (void) const noexcept                   { return this->m_data.max_size();   }
    inline constexpr bool                       empty               (void) const noexcept                   { return this->m_data.empty();      }
    //
    //
    //                                      ACCESSOR FUNCTIONS:
    inline reference                            at                  (index_type idx)                        { return this->m_data.at(idx);      }
    inline const_reference                      at                  (index_type idx) const                  { return this->m_data.at(idx);      }
    //
    inline reference                            front               (void)                                  { return this->m_data.front();      }
    inline const_reference                      front               (void) const                            { return this->m_data.front();      }
    inline reference                            back                (void)                                  { return this->m_data.back();       }
    inline const_reference                      back                (void) const                            { return this->m_data.back();       }
    //
    inline pointer                              data                (void) noexcept                         { return this->m_data.data();       }
    inline const_pointer                        data                (void) const noexcept                   { return this->m_data.data();       }
    //
    //
    //
    //                                      ITERATOR FUNCTIONS:
    inline constexpr iterator                   begin               (void) noexcept                         { return m_data.begin();            }   //  Iterators (enable C++20 ranges/views pipelines)
    inline constexpr const_iterator             begin               (void) const noexcept                   { return m_data.begin();            }
    inline constexpr const_iterator             cbegin              (void) const noexcept                   { return m_data.cbegin();           }
    inline constexpr reverse_iterator           rbegin              (void) const noexcept                   { return m_data.rbegin();           }
    inline constexpr const_reverse_iterator     crbegin             (void) const noexcept                   { return m_data.crbegin();          }
    //
    //
    inline constexpr iterator                   end                 (void) noexcept                         { return m_data.end();              }
    inline constexpr const_iterator             end                 (void) const noexcept                   { return m_data.end();              }
    inline constexpr const_iterator             cend                (void) const noexcept                   { return m_data.cend();             }
    inline constexpr reverse_iterator           rend                (void) const noexcept                   { return m_data.rend();             }
    inline constexpr const_reverse_iterator     crend               (void) const noexcept                   { return m_data.crend();            }
    //
    //
    //                                      RANGES FUNCTIONS:

//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EnumArray" INLINE STRUCT DEFINITION.



/*template<typename E, typename T, std::size_t N = static_cast<std::size_t>(E::Count)>    //  We may need to change from (E::Count) to (E::COUNT)  //
struct EnumArray {
    std::array<T, N>        data;
    T &                     operator []         (E e) noexcept              { return data[static_cast<std::size_t>(e)]; }
    const T &               operator []         (E e) const noexcept        { return data[static_cast<std::size_t>(e)]; }
};*/



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MISC. UTILITIES".






// *************************************************************************** //
//
//
//
//      2.      OTHER...
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



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "OTHER".












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
