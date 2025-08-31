/***********************************************************************************
*
*       ********************************************************************
*       ****             E N T I T I E S . H  ____  F I L E             ****
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
#ifndef _CB_FDTD_ENTITIES_H
#define _CB_FDTD_ENTITIES_H  1
#include CBAPP_USER_CONFIG



//      1.      SUBSIDIARY HEADER FILES...
// *************************************************************************** //
// *************************************************************************** //

//              1A.     INDIVIDUAL FILES.
#ifndef _CB_FDTD_ENTITIES_SOURCES_H                         //  1.  SOURCES.
# include "fdtd/entities/_sources.h"
#endif	// _CB_FDTD_ENTITIES_SOURCES_H  //


#ifndef _CB_FDTD_ENTITIES_BOUNDARIESS_H                     //  2.  BOUNDARY CONDITIONS.
# include "fdtd/entities/_boundaries.h"
#endif	// _CB_FDTD_ENTITIES_BOUNDARIESS_H  //


#ifndef _CB_FDTD_ENTITIES_MATERIALS_H                       //  3.  MATERIALS.
# include "fdtd/entities/_boundaries.h"
#endif	// _CB_FDTD_ENTITIES_MATERIALS_H  //


#ifndef _CB_FDTD_ENTITIES_MEASUREMENTS_H                    //  4.  MEASUREMENTS.
# include "fdtd/entities/_measurements.h"
#endif	// _CB_FDTD_ENTITIES_MEASUREMENTS_H  //


#ifndef _CB_FDTD_ENTITIES_GRIDS_H                           //  5.  GRID.
# include "fdtd/entities/_grid.h"
#endif	// _CB_FDTD_ENTITIES_GRIDS_H  //



//
//
// *************************************************************************** //
// *************************************************************************** //   END "IMPORTS".












#endif      //  _CB_FDTD_ENTITIES_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
