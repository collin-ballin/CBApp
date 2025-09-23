/***********************************************************************************
*
*       ********************************************************************
*       ****                 M A T H . H  ____  F I L E                 ****
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
#ifndef _CBLIB_MATH_H
#define _CBLIB_MATH_H 1

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



//      1.      THIS MODULE'S HEADER FILES...
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBLIB_MATH_TYPES_H
# include "templates/math/_types.h"
#endif	// _CBLIB_MATH_TYPES_H  //
	
#ifndef _CBLIB_MATH_UTILITY_H
# include "templates/math/_utility.h"
#endif	// _CBLIB_MATH_UTILITY_H  //



#ifndef _CBLIB_MATH_VECTORS_H
# include "templates/math/_vectors.h"
#endif	// _CBLIB_MATH_VECTORS_H  //
	
#ifndef _CBLIB_MATH_BEZIER_H
# include "templates/math/_bezier.h"
#endif	// _CBLIB_MATH_BEZIER_H  //

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "MODULE HEADERS".






// *************************************************************************** //
// *************************************************************************** //
#endif  //  _CBLIB_MATH_H  //
