//
//  _config.h
//  CBApp
//
//  Created by Collin Bond on 4/23/25.
//
// *************************************************************************** //
// *************************************************************************** //
#pragma once



// *************************************************************************** //
//  1.  MACROS DEFINED BY "DEAR IMGUI" OR OTHER LIBRARIES]...
//
//
//      *****************************************************
//      ****           D O   N O T   T O U C H           ****
//      *****************************************************
//
// *************************************************************************** //
// *************************************************************************** //
#define     GL_SILENCE_DEPRECATION                      1               //  MACRO DEFINED BY "DEAR IMGUI" EXAMPLE PROGRAM.

//  #define     IMPLOT_DISABLE_OBSOLETE_FUNCTIONS           1
//  #define     IMGUI_DISABLE_OBSOLETE_FUNCTIONS            1

// IMGUI_DISABLE_DEBUG_TOOLS   //  Used to be named "IMGUI_DISABLE_METRICS_WINDOW"

// *************************************************************************** //
// *************************************************************************** //
//  END.






// *************************************************************************** //
//
//
//  2.  MY-OWN GLOBALLY DEFINED PRE-PROCESSOR MACROS...
// *************************************************************************** //
// *************************************************************************** //


//      2.1     CRITICAL BUILD SETTINGS...
// *************************************************************************** //
#define         CBAPP_VERSION                                   "0.03 WIP"


//#define         __CBAPP_DEBUG__                             1                 //  Enable delay before starting int main (bug-fix for X-Code IDE issue).
// #define         __CBAPP_BUILD_CCOUNTER_APP__                    1               //  BUILD FOR COINCIDENCE COUNTER...
#define         __CBAPP_BUILD_FDTD_APP__                        1               //  BUILD FOR FDTD APP...


//#define         __CBAPP_DISABLE_FDTD__                          1           //  BUILD FOR COINCIDENCE COUNTER...


//  ENFORCE THAT THERE ARE NOT MULTIPLE BUILDS ARE #DEFINED...
#if defined(__CBAPP_BUILD_CCOUNTER_APP__) && defined(__CBAPP_BUILD_FDTD_APP__)
    static_assert(false, "Cannot build with more than one __CBAPP_BUILD_APP__ defined (look in \"_config.h\").");
#endif



//      2.2     W-I-P APPLICATION OPTIONS...
// *************************************************************************** //
//#define     CBAPP_ETCH_A_SKETCH                             1           //  DELETE THIS LATER...


//      2.3     DEBUGGING UTILITIES...
// *************************************************************************** //
//#define     CBAPP_USE_VIEWPORT                          1               //  Debug util.  | Should be removed soon (as of 25th/04/2025).
//#define     CBAPP_USE_DOCKSPACE                         1               //  Debug util.  | Should be removed soon (as of 25th/04/2025).
#define     CBAPP_NEW_DOCKSPACE                             1
//#define     CBAPP_ENABLE_MOVE_AND_RESIZE                    1           //  Allow user to moving / resizing core windows.
#define     CBAPP_USE_FONTSCALE_DPI                         1           //  Use "S.m_dpi_fontscale" SEPERATE FROM "S.m_dpi_scale".


//#define     CBAPP_DISABLE_INI                         1               //  Prevent use of ".ini" file to save/recall data.
#define     CBAPP_LOAD_STYLE_FILE                           1           //  Prevent use of ".json" file to save/recall "ImGui" style data.











// *************************************************************************** //
//
//
//  3.  DEBUG BUILD.                    | __CBAPP_DEBUG__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBAPP_DEBUG__
// *************************************************************************** //
//
//


    //#define     CBAPP_ENABLE_CB_DEMO                    1               //  Enable the "CBDemo" window with the application.
    
    
//
//
// *************************************************************************** //
#endif  //  __CBAPP_DEBUG__  //



//      3A.     __CBAPP_DEBUG__  OR  __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
// *************************************************************************** //

#if defined(__CBAPP_DEBUG__) || defined(__CBLIB_RELEASE_WITH_DEBUG_INFO__)
// *************************************************************************** //
//
//
    #define __CBAPP_LOG__
//
//
// *************************************************************************** //
#endif  //  __CBAPP_DEBUG__  ||  __CBLIB_RELEASE_WITH_DEBUG_INFO__












// *************************************************************************** //
//
//
//  4.  RELEASE BUILD.                  | __CBAPP_RELEASE__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBAPP_RELEASE__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBAPP_RELEASE__  //












// *************************************************************************** //
//
//
//  5.  RELEASE WITH DEBUG INFO.        | __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBLIB_RELEASE_WITH_DEBUG_INFO__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBLIB_RELEASE_WITH_DEBUG_INFO__  //












// *************************************************************************** //
//
//
//  6.  MINIMUM SIZE RELEASE BUILD.     | __CBLIB_MIN_SIZE_RELEASE__ MACROS...
// *************************************************************************** //
// *************************************************************************** //

#ifdef __CBLIB_MIN_SIZE_RELEASE__
// *************************************************************************** //
//
//

    //
    //      ...
    //
    
//
//
// *************************************************************************** //
#endif  //  __CBLIB_MIN_SIZE_RELEASE__  //









// *************************************************************************** //
// *************************************************************************** //
//
//  END.
