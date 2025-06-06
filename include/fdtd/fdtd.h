/***********************************************************************************
*
*       ********************************************************************
*       ****                 F D T D . H  ____  F I L E                 ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      May 18, 2025.
*
*       ********************************************************************
*                FILE:      [include/fdtd.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_H
#define _CB_FDTD_H  1



//#define            CB_BAREBONES                       1
#define                _CBAPP_DISABLE_FDTD_FILE_IO            1



#ifndef _GLIBCXX_IOSTREAM
#    include <iostream>
#endif    /*  _GLIBCXX_IOSTREAM  */

#ifndef _GLIBCXX_CMATH
#    include <cmath>
#endif    /*  _GLIBCXX_CMATH  */

#ifndef _GLIBCXX_ARRAY
#    include <array>
#endif    /*  _GLIBCXX_ARRAY  */

#ifndef _GLIBCXX_STDEXCEPT
#    include <stdexcept>
#endif    /*  _GLIBCXX_STDEXCEPT  */

#if __cplusplus >= 201103L
#    ifndef _GLIBCXX_INITIALIZER_LIST
#         include <initializer_list>
#    endif    /*  _GLIBCXX_INITIALIZER_LIST  */
#endif    // C++11.

#ifndef _GLIBCXX_UTILITY
#    include <utility>
#endif    /*  _GLIBCXX_UTILITY  */

#ifndef _GLIBCXX_CSTRING
#    include <cstring>
#endif    /*  _GLIBCXX_CSTRING  */

#ifndef _GLIBCXX_CSTDINT
#    include <cstdint>
#endif    /*  _GLIBCXX_CSTDINT  */

#ifndef _LIBCPP_FILESYSTEM
#    include <filesystem>
#endif     /*    _LIBCPP_FILESYSTEM    */

#ifndef _GLIBCXX_COMPLEX
#    include <complex>
#endif     /*     _GLIBCXX_COMPLEX    */

#ifndef _GLIBCXX_TYPEINFO
#    include <typeinfo>
#endif     /*     _GLIBCXX_TYPEINFO    */


//  #ifndef _CB_FDTD_SOURCES_H
//  #    include <sources.h>
//  #endif /*    _CB_FDTD_SOURCES_H    */


/*#ifndef _CBLIB_BINARY_FILE_H
#    include "../../../../misc/binary_file.h"
#endif */ /*    _CBLIB_BINARY_FILE_H    */






namespace cb { namespace spc {//     BEGINNING NAMESPACE "cb" :: "spc"...
// *************************************************************************** //
// *************************************************************************** //

constexpr long double        pi                         = 3.141592653589793L;
constexpr long double        c                          = 2.997924580000000e+08L;
constexpr long double        eps_0                      = 8.854187812800001e-12L;
constexpr long double        mu_0                       = 1.256637062120000e-06L;
constexpr long double        eta_0                      = 3.767303136680000e+02L;
constexpr long double        electron_mass              = 9.109383701500001e-31L;
constexpr long double        elementary_charge          = 1.602176634000000e-19L;
constexpr long double        electron_volt              = 1.602176634000000e-19L;


// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "spc" NAMESPACE.













#endif      //  _CB_FDTD_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
