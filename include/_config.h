//
//  _config.h
//  CBApp
//
//  Created by Collin Bond on 4/23/25.
//
// *************************************************************************** //
// *************************************************************************** //
#pragma once



//  0.  MACROS DEFINED BY "DEAR IMGUI" OR OTHER LIBRARIES]...
// *************************************************************************** //
//      ****           D O   N O T   T O U C H           ****
// *************************************************************************** //
#define     GL_SILENCE_DEPRECATION                      1               //  MACRO DEFINED BY "DEAR IMGUI" EXAMPLE PROGRAM.






// *************************************************************************** //
//
//
//  1.  MY-OWN GLOBALLY DEFINED PRE-PROCESSOR MACROS...
// *************************************************************************** //
// *************************************************************************** //


//      1.1     CRITICAL BUILD SETTINGS...
// *************************************************************************** //
//#define     __CBAPP_DEBUG__                             1               //  Enable delay before starting int main (bug-fix for X-Code IDE issue).



//      1.2     W-I-P APPLICATION OPTIONS...
// *************************************************************************** //
#define     __CBAPP_THROTTLE_PERF_METRICS__             30ULL           //  Compute performance metrics (FPS, memory-use, etc) every "N"-frames
                                                                        //  (instead of every single frame).  This macro serves as BOTH the ENABLE
                                                                        //  CLAUSE for the feature *AND* the number, N, for the throttle.



//      1.3     DEBUGGING UTILITIES...
// *************************************************************************** //
//#define     CBAPP_USE_VIEWPORT                          1               //  Debug util.  | Should be removed soon (as of 25th/04/2025).
//#define     CBAPP_USE_DOCKSPACE                         1               //  Debug util.  | Should be removed soon (as of 25th/04/2025).
#define         CBAPP_NEW_DOCKSPACE                     1


//      1.4     COMPILE-TIME APPLICATION OPTIONS...
// *************************************************************************** //
#ifdef __CBAPP_DEBUG__
    // #define     CBAPP_ENABLE_CB_DEMO                    1               //  Enable the "CBDemo" window with the application.
    #define     CBAPP_ENABLE_MOVE_AND_RESIZE            1               //  Allow user to MOVE/RESIZE main application windows.
#endif  //  __CBAPP_DEBUG__  //

//#define     CBAPP_DISABLE_INI                         1               //  Prevent use of ".ini" file to save/recall data.
#define     CBAPP_LOAD_STYLE_FILE                       1               //  Prevent use of ".json" file to save/recall "ImGui" style data.
//#define     CBAPP_DISABLE_CUSTOM_FONTS                  1







// *************************************************************************** //
// *************************************************************************** //
//
//  END.
