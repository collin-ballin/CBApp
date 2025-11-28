/***********************************************************************************
*
*       ********************************************************************
*       ****         _ R I N G _ B U F F E R . H  ____  F I L E         ****
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
#ifndef _CBLIB_CONTAINERS_RINGBUFFER_H
#define _CBLIB_CONTAINERS_RINGBUFFER_H 1

//  1.  SYSTEM HEADERS...
#include <iostream>
#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <utility>
#include <cstddef>

#include <iterator>
#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



//  2.  MY HEADERS...
//  #include "templates/utility/type_traits.h"
//  #include "templates/utility/_tags.h"
//  #include "templates/utility/_strings.h"
//  #include "templates/utility/_time.h"
//  #include "templates/utility/_helper.h"



namespace cblib {   //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//
//      1.      GENERAL STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "RingBuffer"
//
/// @struct     RingBuffer
/// @brief      Highly optimized templated ring buffer for real-time plotting.
///
/// @tparam     T, template type parameter, type of stored elements.
/// @tparam     N, template non-type parameter, buffer capacity.
//
template<typename T, std::size_t N>
struct RingBuffer
{
    static_assert(N > 0, "RingBuffer: Buffer size, N, must be positive");

    // *************************************************************************** //
    //      0. |    NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    //                              STANDARD-LIBRARY CONTAINER TYPEDEFS:
    using                               value_type                      = T;
    using                               size_type                       = std::size_t;
    using                               difference_type                 = std::ptrdiff_t;
    using                               reference                       = T &;                  //  now _mutable_
    using                               const_reference                 = const T &;
    using                               pointer                         = T *;
    using                               const_pointer                   = const T *;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //  static constexpr float              ms_MY_CONSTEXPR_VALUE           = 240.0f;
    
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      0. |    INTERNAL TYPES.
    // *************************************************************************** //
    struct iterator {
    //
    //                          TYPENAME ALIASES:
        using                       iterator_category       = std::forward_iterator_tag;
        using                       value_type              = T;
        using                       difference_type         = std::ptrdiff_t;
        using                       pointer                 = const T *;
        using                       reference               = const T &;
    //
    //                          DATA MEMBERS:
        const RingBuffer *          rb                      ;
        size_type                   idx                     ;
    //
    //
    //                          MEMBER FUNCTIONS:
        inline                      iterator                (const RingBuffer * r, size_type i) noexcept : rb(r), idx(i)    {   }
    //
        inline reference            operator*               (void) const noexcept                       { return (*rb)[idx]; }
        inline iterator &           operator++              (void) noexcept                             { ++idx; return *this; }
        inline iterator             operator++              (int) noexcept                              { iterator tmp = *this; ++(*this); return tmp; }
        inline bool                 operator==              (const iterator & rhs) const noexcept       { return idx == rhs.idx; }
        inline bool                 operator!=              (const iterator & rhs) const noexcept       { return idx != rhs.idx; }
    };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "0.  CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      1. |    STORAGE AND STATE VARIABLES.
    // *************************************************************************** //
    T                                   buf                             [N];                //  fixed-size storage (no heap)
    size_type                           head                            { 0 };              //  index of next write (also "oldest" when full)
    size_type                           count                           { 0 };              //  number of valid elements (≤ N)
    
//
// *************************************************************************** //
// *************************************************************************** //   END "1.  DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        MyStruct                (app::AppState & );             //  Def. Constructor.
    //                                  MyStruct                (void) noexcept                 = default;
    //                                  ~MyStruct               (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    //                                  MyStruct                (const MyStruct &    src)       = delete;   //  Copy. Constructor.
    //                                  MyStruct                (MyStruct &&         src)       = delete;   //  Move Constructor.
    //  MyStruct &                      operator =              (const MyStruct &    src)       = delete;   //  Assgn. Operator.
    //  MyStruct &                      operator =              (MyStruct &&         src)       = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2A.  MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      2.B. |  CAPACITY HELPER-FUNCTIONS.
    // *************************************************************************** //
    [[nodiscard]] inline bool                       full            (void) const noexcept       { return count == N;    }
    [[nodiscard]] inline size_type                  size            (void) const noexcept       { return count;         }
    [[nodiscard]] inline bool                       empty           (void) const noexcept       { return count == 0;    }
    [[nodiscard]] inline constexpr size_type        capacity        (void) const noexcept       { return N;             }
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.B. |  ELEMENT ACCESSORS.
    // *************************************************************************** //
    [[nodiscard]] inline reference                  operator[]      (size_type idx) noexcept
    {
        // UB if caller violates pre-condition (idx < size())
        size_type start = full() ? head : 0;
        size_type pos   = start + idx;
        if (pos >= N) pos -= N;
        return buf[pos];
    }

    [[nodiscard]] inline const_reference            operator[]      (size_type idx) const noexcept
    {
        size_type start = full() ? head : 0;
        size_type pos   = start + idx;
        if (pos >= N) pos -= N;
        return buf[pos];
    }

    [[nodiscard]] inline reference                  at              (size_type idx)
    {
        if (idx >= count)
            throw std::out_of_range{"RingBuffer::at – index out of range"};
        return (*this)[idx];
    }

    [[nodiscard]] inline const_reference            at              (size_type idx) const
    {
        if (idx >= count)
            throw std::out_of_range{"RingBuffer::at – index out of range"};
        return (*this)[idx];
    }

    [[nodiscard]] inline reference                  front           (void)       noexcept       { return (*this)[0]; }
    [[nodiscard]] inline const_reference            front           (void) const noexcept       { return (*this)[0]; }

    // back()
    [[nodiscard]] inline reference                  back            (void)       noexcept       { return (*this)[count - 1]; }
    [[nodiscard]] inline const_reference            back            (void) const noexcept       { return (*this)[count - 1]; }

    // top() – most recently pushed element (alias of back)
    [[nodiscard]] inline reference                  top             (void)       noexcept       { return back(); }
    [[nodiscard]] inline const_reference            top             (void) const noexcept       { return back(); }
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.B. |  DATA MODIFIERS.
    // *************************************************************************** //
    inline void                                     clear           (void) noexcept             { head = 0; count = 0; }

    //  "push_back"     (lvalue)
    inline void                                     push_back       (const_reference item) noexcept
    {
        this->buf[this->head]       = item;
        this->head                  = (this->head + 1 == N)     ? 0 :   this->head + 1;
        if ( !this->full() )        { ++this->count; }
    }

    //  "push_back"     (rvalue)
    inline void                                     push_back       (T && item) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        this->buf[this->head]       = std::move(item);
        this->head                  = (this->head + 1 == N)     ? 0 :   this->head + 1;
        if ( !this->full() )        { ++this->count; }
    }

    //  "emplace_back"
    template<typename... Args>
    inline void                                     emplace_back    (Args &&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        this->buf[this->head]       = T(std::forward<Args>(args)...);
        this->head                  = (this->head + 1 == N)     ? 0 :   this->head + 1;
        if ( !this->full() )        { ++this->count; }
    }
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.B. |  RAW ACCESSORS  [ used for APIs like ImGui, ImPlot, etc -- pointer+stride overload ].
    // *************************************************************************** //
    [[nodiscard]] inline const_pointer              raw             (void) const noexcept   { return buf; }
    [[nodiscard]] inline size_type                  offset          (void) const noexcept   { return full() ? head : 0; }
    

    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      2.B. |  ITERATOR SUPPORT.
    // *************************************************************************** //
    [[nodiscard]] inline iterator                   begin           (void) const noexcept               { return iterator(this, 0);        }
    [[nodiscard]] inline iterator                   end             (void)   const noexcept             { return iterator(this, count);    }
    
//
// *************************************************************************** //
// *************************************************************************** //   END "2B.  INLINE" FUNCTIONS.



//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "RingBuffer" INLINE STRUCT DEFINITION.






//
//
//
// *************************************************************************** //
// *************************************************************************** //   END [[ 1.  "RING BUFFER" ]].












// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_CONTAINERS_RINGBUFFER_H  //
