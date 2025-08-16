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
*                FILE:      [include/fdtd.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CB_FDTD_TYPES_H
#define _CB_FDTD_TYPES_H  1


//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "fdtd/_objects.h"


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



// *************************************************************************** //
//
//
//      0.      SIMULATION TYPES...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//      1.      SIMULATION TYPES...
// *************************************************************************** //
// *************************************************************************** //

//  "StepSizes"
//      FDTD Struct for SIMULATION STEPSIZES...
template<typename T>
struct StepSizes {
    CBAPP_CBLIB_TYPES_API           //  BRING-IN TYPENAME ALIASES FOR TYPES DEFINED IN "CBLIB".
//
    Param<T>        dx;
    Param<T>        dy;
    Param<T>        dz;
    Param<T>        dt;
    Param<T>        Sc;
};


//  "Steps"
//      FDTD structure for NUMBER OF SIMULATION STEPS...
template<typename T>
struct Steps {
    CBAPP_CBLIB_TYPES_API           //  BRING-IN TYPENAME ALIASES FOR TYPES DEFINED IN "CBLIB".
//
    Param<T>        NX;
    Param<T>        NY;
    Param<T>        NZ;
    Param<T>        NT;
};


//  "Parameters"
//      FDTD structure for NUMBER OF SIMULATION STEPS...
template<typename I, typename F>
struct Parameters {
    CBAPP_CBLIB_TYPES_API           //  BRING-IN TYPENAME ALIASES FOR TYPES DEFINED IN "CBLIB".
//
    Param<I>        wavelength;
    Param<I>        duration;
};


// *************************************************************************** //
// *************************************************************************** //   END "Simulation Types".












// *************************************************************************** //
//
//
//      2.      FDTD PAYLOAD TYPES...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
// *************************************************************************** //   END "Payload Types".









// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" :: "fdtd" NAMESPACE.













#endif      //  _CB_FDTD_TYPES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
