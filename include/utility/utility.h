//
//  utility.h
//  CBApp
//
//  Created by Collin Bond on 4/16/25.
//
// *************************************************************************** //
// *************************************************************************** //
#ifndef _CBAPP_UTILITY_H
#define _CBAPP_UTILITY_H  1

#include <stdio.h>
#include <vector>
#include <thread>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <math.h>


#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "json.hpp"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers


#include "utility/_constants.h"
#include "utility/_templates.h"



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //




//  1.  DEFINED IN "utility.cpp"...
// *************************************************************************** //
// *************************************************************************** //

// *************************************************************************** //
//
//
//  0.  INLINE DEFINITIONS (HEADER ONLY)...
// *************************************************************************** //
// *************************************************************************** //

//  "WindowLocation"
enum class WindowLocation {
    Center,         // default
    LeftHalf,       // left half of the monitor
    RightHalf       // right half of the monitor
};






// *************************************************************************** //
//
//
//  1.  DEFINED IN "utility.cpp"...
// *************************************************************************** //
// *************************************************************************** //

//      1.1     MISC / UTILITY FUNCTIONS...
// *************************************************************************** //
void                            glfw_error_callback         (int error, const char * description);
void                            HelpMarker                  (const char* desc);


//      1.2     WINDOW SIZE / GEOMETRY FUNCTIONS...
// *************************************************************************** //
[[nodiscard]]
GLFWmonitor *                   get_current_monitor         (GLFWwindow * window);
void                            set_window_scale            (GLFWwindow * window, const float scale);
void                            set_next_window_geometry    (GLFWwindow * glfw_window, float pos_x_frac, float pos_y_frac,
                                                                                       float width_frac, float height_frac);

void                            SetGLFWWindowLocation       (GLFWwindow *   win, const      WindowLocation          loc,
                                                            const float     scale=0.5f,     const GLFWmonitor *     monitor=nullptr);


//      1.3     CONTEXT CREATION / INITIALIZATION FUNCTIONS...
// *************************************************************************** //
const char *                    get_glsl_version            (void);



//      1.4     MISC I/O FUNCTIONS...
// *************************************************************************** //
                                //  1.4A-1      SAVING/WRITING FUNCTIONS...
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  const std::string &         file_path);
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  const char *                file_path);
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  std::string_view            file_path);

                                //  1.4A-2      ASYNCHRONUS SAVING/WRITING FUNCTIONS...
bool                            SaveStyleToDiskAsync        (const ImGuiStyle &     style,  const char *                file_path);
bool                            SaveStyleToDiskAsync        (ImGuiStyle             style,  const char *                file_path);


                                //  1.4B-1      LOADING FUNCTIONS...
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const char *                file_path);
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const std::string &         file_path);
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const std::string_view &    file_path);

                                //  1.4B-2      ASYNCHRONUS LOADING FUNCTIONS...


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

//      2.1     PLOTTING STUFF...
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
[[nodiscard]] std::vector<ImVec4>   GetColormapSamples          (int M, ImPlotColormap map);






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
