/***********************************************************************************
*
*       ********************************************************************
*       ****               _ T Y P E S . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      June 15, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_TYPES_H
#define _CBLIB_MATH_TYPES_H 1

#include <iostream>
#include <type_traits>
#include <algorithm>
#if __cpp_concepts >= 201907L
# include <concepts>
#endif  //  C++20.  //

#include <cmath>
#include <cstddef>
#include <iomanip>

#include <vector>
#include <array>
#if __cplusplus >= 201103L
# include <initializer_list>
#endif	//  C++11.  //



namespace cblib { namespace math {   //     BEGINNING NAMESPACE "cblib" :: "math"...
// *************************************************************************** //
// *************************************************************************** //



//  1.  GENERAL MATH TYPES AND DATA ABSTRACTIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "Range"
//      - 1.
template<typename T>
struct Range {
    T               min,    max;
};


//  "Param"
//      - 2.    A parameter that carries both a value and its valid range
template<typename T>
struct Param {
//
//                          INITIALIZATION FUNCTIONS...
//
//
//                          OVERLOADED OPERATORS...
    inline T &                  Value                   (void)      { return value;         }
    inline T &                  Min                     (void)      { return limits.min;    }
    inline T &                  Max                     (void)      { return limits.max;    }
//
    inline T &                  RangeMin                (void)      { return limits.min;    }
    inline T &                  RangeMax                (void)      { return limits.max;    }
//
//
//                          DATA MEMBERS...
    T                           value;
    Range<T>                    limits;
};








// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_TYPES_H  //
