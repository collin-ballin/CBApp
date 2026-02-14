/**
 * @file    include/main/_init.h
 * @author  Collin A. Bond
 * @date    2026-02-13
 * @brief   build-settings for `cblib` library.
 *
 *
 * @details     this file contains all the stuff for compile-time configurations and build-settings for the `cblib` library.
 *
 * @note        EMPTY
 * @warning     EMPTY
 *
 *
 * @todo        EMPTY
 *
 *
 *
 * @since       EMPTY
 * @see         EMPTY
 * @deprecated  EMPTY
 */
/***********************************************************************************
*
*       ********************************************************************
*       ****           M A I N / _ I N I T . H  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      February 13, 2025.
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBLIB_INIT_H
#define	_CBLIB_INIT_H	1



//      0.      DOCUMENTATION...
// *************************************************************************** //
// *************************************************************************** //



/* ***********************************************************************************************************************
 *      M I S C .    I N F O . . .
 * ***********************************************************************************************************************
 *
 *      "__cplusplus"  encodes the standard as  "yyyymmL".
 *      EX:     201103L     = 2011-03   = C++11.            ===> So the code includes <initializer_list> when compiling in C++11+.
 *
 *      ╔═══════════════╤═══════════════╤═══════════════════════════════╗
 *      ║   199711L     │   C++98/03    │                               ║
 *      ║               │               │                               ║
 *      ║   201103L     │   C++11       │                               ║
 *      ║               │               │                               ║
 *      ║   201402L     │   C++14       │                               ║
 *      ║               │               │                               ║
 *      ║   201703L     │   C++17       │                               ║
 *      ║               │               │                               ║
 *      ║   202002L     │   C++20       │                               ║
 *      ║               │               │                               ║
 *      ║   202302L     │   C++23       │                               ║
 *      ╚═══════════════╧═══════════════╧═══════════════════════════════╝
 *
 * ******************************************************************************************************************** */



/* ***********************************************************************************************************************
 *      T A B L E    O F    B U I L D - S E T T I N G S . . .
 * ***********************************************************************************************************************
 *
 *      ╔══════════════════════════════════════════════════════════╤═══════════════════════════════════════════════════╗
 *      ║ `-D` include name (from compiler)                        │ Reserved Internal Name (for `cblib`)              ║
 *      ╟──────────────────────────────────────────────────────────┼───────────────────────────────────────────────────╢
 *      ║  CBLIB_DISABLE_THIRD_PARTY_INCLUDES                      │   _CBLIB_NO_DEPENDENCIES                          ║
 *      ║  _CBLIB_INTERNAL_PLACEHOLDER_1                           │   _CBLIB_INTERNAL_PLACEHOLDER_1                   ║
 *      ║                                                          │                                                   ║
 *      ║  -DNAME                                                  │   _CBLIB_NAME                                     ║
 *      ║  -DNAME                                                  │   _CBLIB_NAME                                     ║
 *      ╚══════════════════════════════════════════════════════════╧═══════════════════════════════════════════════════╝
 *
 *
 * ******************************************************************************************************************** */







// *************************************************************************** //
//
//
//
//      1.      COMPILE-TIME MACRO ENFORCEMENT.
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      1A. BUILD. |    INVENTORY OF ALL BUILD OPTIONS.
// *************************************************************************** //

//  ENFORCE THAT NONE OF THE RESERVED / INTERNAL MACROS-NAMES ARE `#defined`...
//
#if defined(_CBLIB_NO_DEPENDENCIES)                     ||  \
    defined(_CBLIB_INTERNAL_PLACEHOLDER_1)              ||  \
    defined(_CBLIB_INTERNAL_PLACEHOLDER_2)              ||  \
    defined(_CBLIB_INTERNAL_PLACEHOLDER_3)
//
    #error      "fatal error: one or more reserved macro-names have been `-D` defined.\n"       \
                "(`cblib` requires that certain macro names are UN-DEFINED at compilation-"     \
                "time because these macros are used internally by our library)."
//
#endif  //  ANY_RESERVED_MACROS  //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "1. MACRO-ENFORCEMENT".












// *************************************************************************** //
//
//
//
//      2.      BUILD-SETTINGS AND BEHAVIORS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2B. BUILD. |    EMPTY.
// *************************************************************************** //

//              2.1.        CBLIB_DISABLE_THIRD_PARTY_INCLUDES      - DISABLE the usage of any `#includes "..."` that depend on third-party libraries (e.g.: imgui, json, etc).
//
#ifdef CBLIB_DISABLE_THIRD_PARTY_INCLUDES
//
    # pragma message    ("building without third-party libraries (-DCBLIB_DISABLE_THIRD_PARTY_INCLUDES)")   // #warning "string..."     --- `#warning` is a cpp-23 extension.
    # define            _CBLIB_NO_DEPENDENCIES      1
//
#endif  //  CBLIB_DISABLE_THIRD_PARTY_INCLUDES  //



//              2.2.        CBAPP_USER_CONFIG                       - Set the directory of where the user configuration header is at.
//
#ifndef CBAPP_USER_CONFIG
  #     error           "fatal error: CBLib requires preprocessor macro CBAPP_USER_CONFIG to specify build configurations."
#endif  //  CBAPP_USER_CONFIG  //



//              2.3.        __cplusplus                             - C++ STANDARD ENFORCEMENT.
//
#if __cplusplus < 201907L
  #     error   "fatal error: CBLib is required to be build with \"cpp 201907L\" or greater.\n"     \
                "include one of the following compiler flags:\n\t"                                  \
                "[LLVM Clang]                   : -std=c++20    or      -std=gnu++20    \n\t"       \
                "[MinGW-w64 (g++/clang++)]      : -std=c++20    \n\t"                               \
                "[MSVC (cl.exe)]                : /std:c++20    \n\t"
#endif  //  __cplusplus < 201907L (C++20).  //



//              2.4.        __has_include(<concepts>)               - Need a cpp-standard with cpp-20.
//
#if !__has_include(<concepts>)
  #     error   "<concepts> header not found (need a C++20 standard library)."
#endif  //  !__has_include(<concepts>)  //



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "2. BUILD SETTINGS".
























// *************************************************************************** //
// *************************************************************************** //
#endif 	//  _CBLIB_INIT_H  //
