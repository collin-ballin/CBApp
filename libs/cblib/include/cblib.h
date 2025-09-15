/***********************************************************************************
*
*       ********************************************************************
*       ****                C B L I B . H  ____  F I L E                ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      April 15, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_H
#define	_CBLIB_MAIN_H	1



//      0.      BUILD REQUIREMENTS [NEED TO REFACTOR THIS]...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//      INFO...
// *************************************************************************** //
//
//      "__cplusplus"  encodes the standard as  "yyyymmL".
//      EX:     201103L     = 2011-03   = C++11.            ===> So the code includes <initializer_list> when compiling in C++11+.
//
//          |===============|===============|===============================|
//          |   199711L     |   C++98/03    |                               |
//          |               |               |                               |
//          |   201103L     |   C++11       |                               |
//          |               |               |                               |
//          |   201402L     |   C++14       |                               |
//          |               |               |                               |
//          |   201703L     |   C++17       |                               |
//          |               |               |                               |
//          |   202002L     |   C++20       |                               |
//          |               |               |                               |
//          |   202302L     |   C++23       |                               |
//          |===============|===============|===============================|
//
// *************************************************************************** //

//      1.      COMPILE-TIME MACRO ENFORCEMENT.
#ifndef CBAPP_USER_CONFIG
  #     error   "fatal error: CBLib requires preprocessor macro CBAPP_USER_CONFIG to specify build configurations."
#endif  //  CBAPP_USER_CONFIG  //

//  #ifndef CBLIB_USER_CONFIG
//    #     error   "fatal error: CBLib requires preprocessor macro CBLIB_USER_CONFIG to specify build configurations."
//  #endif  //  CBLIB_USER_CONFIG  //






//      2.      C++ STANDARD ENFORCEMENT.
#if __cplusplus < 201907L
//
  #     error   "fatal error: CBLib is required to be build with \"cpp 201907L\" or greater.\n"     \
                "include one of the following compiler flags:\n\t"                                  \
                "[LLVM Clang]                   : -std=c++20    or      -std=gnu++20    \n\t"       \
                "[MinGW-w64 (g++/clang++)]      : -std=c++20    \n\t"                               \
                "[MSVC (cl.exe)]                : /std:c++20    \n\t"
//
#endif  //  C++20.  //

#if !__has_include(<concepts>)
  #     error   "<concepts> header not found (need a C++20 standard library)."
#endif  //  !__has_include(<concepts>)  //

//  #if __cplusplus >= 201103L
//  # include <initializer_list>
//  #endif	// C++11.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "BUILD REQUIREMENTS".






// *************************************************************************** //
//
//
//      1.      GLOBALLY INCLUDED HEADERS...
// *************************************************************************** //
// *************************************************************************** //

#include "PCH/PCH.h"

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "GLOBAL HEADERS".






// *************************************************************************** //
//
//
//      2.      "CBLIB" LIBRARY MODULES AND HEADER FILES...
// *************************************************************************** //
// *************************************************************************** //

#ifndef _CBLIB_UTILITY_H
# include "main/utility.h"
#endif	// _CBLIB_UTILITY_H  //

#ifndef _CBLIB_UTILITY_UTILITY_H
# include "templates/utility/utility.h"
#endif	// _CBLIB_UTILITY_UTILITY_H  //

#ifndef _CBLIB_MATH_H
# include "templates/math/math.h"
#endif	// _CBLIB_MATH_H  //

#ifndef _CBLIB_CONTAINERS_H
# include "templates/containers/containers.h"
#endif	// _CBLIB_CONTAINERS_H  //

#ifndef _CBLIB_IMGUI_EXTENSIONS_H
# include "templates/imgui_extensions/imgui_extensions.h"
#endif	// _CBLIB_IMGUI_EXTENSIONS_H  //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "LIBRARY MODULES".






// *************************************************************************** //
// *************************************************************************** //
#endif 	//  _CBLIB_MAIN_H  //
