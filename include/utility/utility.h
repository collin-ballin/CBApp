/**
 * @file    utility.h
 * @brief   General utilities for CBApp application.
 * @author  Collin A. Bond
 * @date    2025-04-16
 */
/***********************************************************************************
*
*       ********************************************************************
*       ****           _ U T I L I T Y . C P P  ____  F I L E           ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      April 16, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBAPP_UTILITY_H
#define _CBAPP_UTILITY_H  1



//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "app/_init.h"
#include "utility/_types.h"
#include "utility/_constants.h"
#include "utility/_templates.h"
#include "utility/_logger.h"
#ifdef _WIN32
    # include "utility/resource_loader.h"
#endif  //  _WIN32  //
//#include "utility/pystream/pystream.h"
#include "json.hpp"


//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdio.h>
#include <chrono>
#include <string>
#include <string_view>
#include <iomanip>

#include <vector>
#include <thread>
#include <type_traits>
#include <math.h>


//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //




//  0.1     INLINE DEFINITIONS (HEADER ONLY)        |   WIDGETS & WINDOW STRUCTS...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//
//
//  1.  DEFINED IN "utility.cpp"...
// *************************************************************************** //
// *************************************************************************** //

//      1.1     MISC / UTILITY FUNCTIONS...
// *************************************************************************** //
void                            HelpMarker                  (const char * desc);
const char *                    get_opengl_version          (void);
int                             get_glfw_version            (char * );
inline bool                     GetIO_KeyCtrl               (void) {
                                #ifdef __APPLE__
                                    return ImGui::GetIO().KeyShift;
                                # else
                                    return ImGui::GetIO().KeyCtrl;
                                #endif  //  __APPLE__  //
                                }


//      1.2     WINDOW / GLFW FUNCTIONS...
// *************************************************************************** //
void                            glfw_error_callback         (int error, const char * description);
[[nodiscard]] GLFWwindow *      CreateGLFWWindow            (int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share);



//      1.3     WINDOW SIZE / GEOMETRY FUNCTIONS...
// *************************************************************************** //
[[nodiscard]]
std::tuple<int,int>             GetMonitorDimensions        (GLFWwindow * window);
[[nodiscard]] float             GetDPIScaling               (GLFWwindow * window);
[[nodiscard]] float             GetDPIFontScaling           (GLFWwindow * window);

[[nodiscard]]
GLFWmonitor *                   get_current_monitor         (GLFWwindow * window);
void                            set_window_scale            (GLFWwindow * window, const float scale);
void                            set_next_window_geometry    (GLFWwindow * glfw_window, float pos_x_frac, float pos_y_frac,
                                                                                       float width_frac, float height_frac);

void                            SetGLFWWindowLocation       (GLFWwindow *   win, const      WindowLocation          loc,
                                                             const float    scale=0.5f,     const GLFWmonitor *     monitor=nullptr);
ImVec2                          GetImGuiWindowCoords        (const char * , const Anchor & );


//      1.4     WIDGET FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

//              1.4A    HELPER WIDGET FUNCTIONS / ABSTRACTIONS...


//              1.4B    POP-UP WINDOWS, FILE-DIALOGUE WINDOWS, ETC...
bool                            Popup_Save                  (const char * );
bool                            Popup_AskOkCancel           (const char * );
//
bool                            Popup_AskQuestion           (const char * );
bool                            Popup_AskRetryCancel        (const char * );
bool                            Popup_AskYesNo              (const char * );
bool                            Popup_ShowError             (const char * );
bool                            Popup_ShowInfo              (const char * );
bool                            Popup_ShowWarning           (const char * );
bool                            file_exists                 (const char * );



//              1.4C    GENERAL WIDGET FUNCTIONS...
void                            LeftLabelSimple             (const char * );
void                            LeftLabel2                  (const char * , const float );
void                            LeftLabel2                  (const char * , const float, const float );
void                            LeftLabel                   (const char * ,
                                                             const float label_width = 150.0f,
                                                             const float widget_width = 250.0f);
void                            LeftLabel                   (const char * ,   const ImVec2 & ,  const float );
void                            LeftLabel                   (const char * ,   const ImVec2 & ,  const ImVec2 & );


//      1.5     CONTEXT CREATION / INITIALIZATION FUNCTIONS...
// *************************************************************************** //
const char *                    get_glsl_version            (void);


//      1.6     MISC I/O FUNCTIONS...
// *************************************************************************** //
//              1.6A-1      SAVING/WRITING FUNCTIONS...
                //              1.  ImGui Style:
bool                            SaveImGuiStyleToDisk        (const ImGuiStyle &     style,  const std::string &         file_path);
bool                            SaveImGuiStyleToDisk        (const ImGuiStyle &     style,  const char *                file_path);
bool                            SaveImGuiStyleToDisk        (const ImGuiStyle &     style,  std::string_view            file_path);
                            //
bool                            SaveImGuiStyleToDiskAsync   (const ImGuiStyle &     style,  const char *                file_path);
// bool                          SaveImGuiStyleToDiskAsync   (ImGuiStyle             style,  const char *                file_path);
                //
                //              2.  ImPlot Style:
bool                            SaveImPlotStyleToDisk       (const ImPlotStyle &    style,  const std::string &         file_path);
bool                            SaveImPlotStyleToDisk       (const ImPlotStyle &    style,  const char *                file_path);
bool                            SaveImPlotStyleToDisk       (const ImPlotStyle &    style,  std::string_view            file_path);
                            //
bool                            SaveImPlotStyleToDiskAsync  (const ImPlotStyle &    style,  const char *                file_path);
// bool                         SaveImPlotStyleToDiskAsync  (ImPlotStyle            style,  const char *                file_path);



//              1.6B-1      LOADING FUNCTIONS...
                //              1.  ImGui Style:
bool                            LoadImGuiStyleFromDisk      (ImGuiStyle &           style,  const char *                file_path);
bool                            LoadImGuiStyleFromDisk      (ImGuiStyle &           style,  const std::string &         file_path);
bool                            LoadImGuiStyleFromDisk      (ImGuiStyle &           style,  const std::string_view &    file_path);
                //
                //              2.  ImPlot Style:
bool                            LoadImPlotStyleFromDisk     (ImPlotStyle &         style,  const char *                file_path);
bool                            LoadImPlotStyleFromDisk     (ImPlotStyle &         style,  const std::string &         file_path);
bool                            LoadImPlotStyleFromDisk     (ImPlotStyle &         style,  const std::string_view &    file_path);
                //
                //              3.  ImGui ".ini" File:
bool                            LoadIniSettingsFromDisk     (const char *);

//              1.6B-2      ASYNCHRONUS LOADING FUNCTIONS...
                            //
                            //      ...
                            //
                                

//
//
//
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//  2.  DEFINED IN "plot_utility.cpp"...
// *************************************************************************** //
// *************************************************************************** //

//      2.1     UTILITY...
// *************************************************************************** //
// *************************************************************************** //
int                                 GetPlotItems                (ImGuiID );
int                                 GetPlotItems                (ImPlotPlot * );
std::pair<ImPlotPlot*, ImGuiID>     GetPlot                     (const char * );
ImGuiID                             GetPlotID                   (const char * );
int                                 DisplayAllPlotIDs           (void);


//      2.1     OLD PLOTTING STUFF...
// *************************************************************************** //
// *************************************************************************** //
void                                Sparkline                   (const char * id, const float * values, int count, float min_v, float max_v, int offset, const ImVec4 & color, const ImVec2 & size);
void                                ScrollingSparkline          (const float time, ScrollingBuffer & data, const float window, const ImPlotAxisFlags flags, const float center = 0.75f);
void                                ScrollingSparkline          (const float time,          const float window,                     ScrollingBuffer & data,
                                                                 const float ymin,          const float ymax,                       const ImPlotAxisFlags flags,
                                                                 const ImVec4 & color,      const ImVec2 & size=ImVec2(-1,150),     const float center=0.75f);
void                                ScrollingSparkline          (const float time,              const float window,     ScrollingBuffer & data,
                                                                 const ImPlotAxisFlags flags,   const ImVec4 & color,   const ImVec2 & size=ImVec2(-1,150),
                                                                 const float center=0.75f);

                                                                 
//      2.2     UTILITY FUNCTIONS FOR IMPLOT STUFF...
// *************************************************************************** //
// *************************************************************************** //
[[nodiscard]] std::vector<ImVec4>   GetColormapSamples          (const size_t M);
[[nodiscard]] std::vector<ImVec4>   GetColormapSamples          (int M, const char * cmap);
[[nodiscard]] std::vector<ImVec4>   GetColormapSamples          (int M, ImPlotColormap map);
[[nodiscard]] ImPlotColormap        CreateTransparentColormap   (ImPlotColormap , float , const char * );





// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} }//   END OF "cb" NAMESPACE.












#endif      //  _CBAPP_UTILITY_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
