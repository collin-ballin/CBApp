/***********************************************************************************
*
*       ********************************************************************
*       ****              U T I L I T Y . H  ____  F I L E              ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 25, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_UTILITY_UTILITY_H
#define _CBLIB_UTILITY_UTILITY_H 1

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



//  MY OWN HEADERS...
#include "templates/utility/type_traits.h"
#include "templates/utility/_strings.h"
#include "templates/utility/helper.h"



namespace cblib {   //     BEGINNING NAMESPACE "cblib"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERAL STUFF...
// *************************************************************************** //
// *************************************************************************** //

// Highly optimized templated ring buffer for real-time plotting.
// Template parameters:
//   T - type of stored elements (default: ImVec2)
//   N - capacity (default: 2000)

/// @struct     RingBuffer
/// @brief      Highly optimized templated ring buffer for real-time plotting.
///
/// @tparam     T, template type parameter, type of stored elements.
/// @tparam     N, template non-type parameter, buffer capacity.
template<typename T, std::size_t N>
struct RingBuffer
{
    static_assert(N > 0, "Buffer size N must be positive");

    /*------------------------------------------------------------------
        Standard container typedefs
    ------------------------------------------------------------------*/
    using value_type        = T;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = T &;              // ‼️ now _mutable_
    using const_reference   = const T &;
    using pointer           = T *;
    using const_pointer     = const T *;

    /*------------------------------------------------------------------
        Storage & state
    ------------------------------------------------------------------*/
    T         buf[N];          // fixed-size storage (no heap)
    size_type head  {0};       // index of next write (also "oldest" when full)
    size_type count {0};       // number of valid elements (≤ N)

    [[nodiscard]] inline bool full() const noexcept { return count == N; }

    /*------------------------------------------------------------------
        Capacity helpers
    ------------------------------------------------------------------*/
    [[nodiscard]] inline size_type size()     const noexcept { return count; }
    [[nodiscard]] inline bool      empty()    const noexcept { return count == 0; }
    [[nodiscard]] inline constexpr size_type capacity() const noexcept { return N; }
    
    /*------------------------------------------------------------------
        Element access (chronological)
        -  operator[] : unchecked (UB if idx ≥ size())
        -  at()       : checked   (throws std::out_of_range)
    ------------------------------------------------------------------*/
    [[nodiscard]] inline reference operator[](size_type idx) noexcept
    {
        // UB if caller violates pre-condition (idx < size())
        size_type start = full() ? head : 0;
        size_type pos   = start + idx;
        if (pos >= N) pos -= N;
        return buf[pos];
    }

    [[nodiscard]] inline const_reference operator[](size_type idx) const noexcept
    {
        size_type start = full() ? head : 0;
        size_type pos   = start + idx;
        if (pos >= N) pos -= N;
        return buf[pos];
    }

    [[nodiscard]] inline reference at(size_type idx)
    {
        if (idx >= count)
            throw std::out_of_range{"RingBuffer::at – index out of range"};
        return (*this)[idx];
    }

    [[nodiscard]] inline const_reference at(size_type idx) const
    {
        if (idx >= count)
            throw std::out_of_range{"RingBuffer::at – index out of range"};
        return (*this)[idx];
    }

    [[nodiscard]] inline reference       front()       noexcept { return (*this)[0]; }
    [[nodiscard]] inline const_reference front() const noexcept { return (*this)[0]; }

    // back()
    [[nodiscard]] inline reference       back()       noexcept { return (*this)[count - 1]; }
    [[nodiscard]] inline const_reference back() const noexcept { return (*this)[count - 1]; }

    // top() – most recently pushed element (alias of back)
    [[nodiscard]] inline reference       top()       noexcept { return back(); }
    [[nodiscard]] inline const_reference top() const noexcept { return back(); }

    /*------------------------------------------------------------------
        Modifiers
    ------------------------------------------------------------------*/
    inline void clear() noexcept { head = 0; count = 0; }

    // push_back (lvalue)
    inline void push_back(const_reference item) noexcept {
        buf[head] = item;
        head = (head + 1 == N) ? 0 : head + 1;
        if (!full()) ++count;
    }
    // push_back (rvalue)
    inline void push_back(T && item) noexcept(std::is_nothrow_move_assignable_v<T>) {
        buf[head] = std::move(item);
        head = (head + 1 == N) ? 0 : head + 1;
        if (!full()) ++count;
    }
    // emplace_back
    template<typename... Args>
    inline void emplace_back(Args &&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        buf[head] = T(std::forward<Args>(args)...);
        head = (head + 1 == N) ? 0 : head + 1;
        if (!full()) ++count;
    }

    /*------------------------------------------------------------------
        Raw access (for APIs like ImPlot pointer+stride overload)
    ------------------------------------------------------------------*/
    [[nodiscard]] inline const_pointer raw()    const noexcept { return buf; }
    [[nodiscard]] inline size_type     offset() const noexcept { return full() ? head : 0; }

    /*------------------------------------------------------------------
        Iterator support (const forward iterator)
    ------------------------------------------------------------------*/
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T *;
        using reference         = const T &;

        const RingBuffer * rb;
        size_type         idx;

        inline iterator(const RingBuffer * r, size_type i) noexcept : rb(r), idx(i) {}
        inline reference operator*() const noexcept { return (*rb)[idx]; }
        inline iterator & operator++() noexcept { ++idx; return *this; }
        inline iterator operator++(int) noexcept { iterator tmp = *this; ++(*this); return tmp; }
        inline bool operator==(const iterator & rhs) const noexcept { return idx == rhs.idx; }
        inline bool operator!=(const iterator & rhs) const noexcept { return idx != rhs.idx; }
    };

    [[nodiscard]] inline iterator begin() const noexcept { return iterator(this, 0); }
    [[nodiscard]] inline iterator end()   const noexcept { return iterator(this, count); }
};








// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cblib" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_UTILITY_UTILITY_H  //
