/***********************************************************************************
*
*       ********************************************************************
*       ****             _ V E C T O R S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 13, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_MATH_VECTORS_H
#define _CBLIB_MATH_VECTORS_H 1

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



//  VECTORS, MATRICES, TENSORS, AND HIGHER DIMENSIONAL OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //



//      1.1     GENERIC / HELPER OPERATIONS...
// *************************************************************************** //

//  "make_real_vector"
//
template< typename complex_t >
inline std::vector<typename complex_t::value_type>
make_real_vector(const std::vector<complex_t> & src)
{
    using T = typename complex_t::value_type;
    std::vector<T> dst; 
    dst.reserve(src.size());

    std::transform(src.begin(), src.end(),
                   std::back_inserter(dst),
                   [](const complex_t& c) { return c.real(); });

    return dst;            // same length, real-valued
}






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cblib" :: "math" NAMESPACE.







// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_VECTORS_H  //
