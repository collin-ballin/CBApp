/***********************************************************************************
*
*       ********************************************************************
*       ****             _ S O U R C E S . H  ____  F I L E             ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      August 31, 2025.
*
*       ********************************************************************
*                FILE:      [include/fdtd.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_ENTITIES_SOURCES_H
#define _CB_FDTD_ENTITIES_SOURCES_H  1


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



//      NOTES...
// *************************************************************************** //
// *************************************************************************** //
//
//  CORE TYPES:
//      SourceType                  : SoftE, HardE, SoftB, HardB.
//      SourceRole                  : Field Source  vs.  TF/SF Directional Source,
//      FieldComponent              : What COMPONENT(S) of the field are generated.
//
//      WaveformType                : Gaussian Pulse  vs.  Ricker Wavelet  vs.  Time-Harmonic.
//
//
//  AUXILIARY TYPES:
//
//
//
//
//
//
// *************************************************************************** //











//      0.      ENUM TYPES...
// *************************************************************************** //
// *************************************************************************** //


//  "SourceType"
enum class SourceType : uint8_t {
    HardE, SoftE, HardB, SoftB,
//
    COUNT
};
//
//  "DEF_SOURCE_TYPE_NAMES"
//      COMPILE-TIME ARRAY CONTAINING THE NAME OF ALL FDTD SOURCE TYPES.
inline static const std::array<const char *, static_cast<size_t>(SourceType::COUNT)>
DEF_SOURCE_TYPE_NAMES        = {{
    "Hard-E",       "Soft-E",        "Hard-B",          "Soft-B"
}};




//  "SourceRole"
enum class SourceRole : uint8_t {
    Field,      Directional,
//
    COUNT
};
//
//  "DEF_SOURCE_ROLE_NAMES"
inline static const std::array<const char *, static_cast<size_t>(SourceRole::COUNT)>
DEF_SOURCE_ROLE_NAMES        = {{
    "Field Source",       "Directional Source"
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













#endif      //  _CB_FDTD_ENTITIES_SOURCES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
