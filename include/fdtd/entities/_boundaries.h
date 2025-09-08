/***********************************************************************************
*
*       ********************************************************************
*       ****          _ B O U N D A R I E S . H  ____  F I L E          ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      August 31, 2025.
*
*       ********************************************************************
*                FILE:      [include/fdtd.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_ENTITIES_BOUNDARIES_H
#define _CB_FDTD_ENTITIES_BOUNDARIES_H  1


//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>
#include <filesystem>
#include <stdexcept>

#include <cmath>
#include <complex>
#include <cstdint>

#include <vector>
#include <array>
#include <initializer_list>

#include <utility>
#include <cstring>

#include <typeinfo>






namespace cb { namespace fdtd {//     BEGINNING NAMESPACE "cb" :: "fdtd"...
// *************************************************************************** //
// *************************************************************************** //



//      0.      ENUM TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "BoundaryType"
//
enum class BoundaryType : uint8_t {
    Mur,
//
    COUNT
};
//
//  "DEF_BOUNDARY_TYPE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL FDTD BOUNDARY CONDITION TYPES.
//
inline static const std::array<const char *, static_cast<size_t>(BoundaryType::COUNT)>
DEF_BOUNDARY_TYPE_NAMES        = {{
    "Mur ABC"
}};






//
//
// *************************************************************************** //
// *************************************************************************** //   END "ENUM Types".






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "fdtd" NAMESPACE.













#endif      //  _CB_FDTD_ENTITIES_BOUNDARIES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
