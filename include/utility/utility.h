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
#include "_config.h"
#include "cblib.h"
#include "utility/_constants.h"
#include "utility/_templates.h"
#include "utility/_logger.h"
#ifdef _WIN32
    # include "utility/resource_loader.h"
#endif  //  _WIN32  //
//#include "utility/pystream/pystream.h"
#include "json.hpp"


//  0.2     STANDARD LIBRARY HEADERS...
#include <stdio.h>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <math.h>


//  0.3     "DEAR IMGUI" HEADERS...
#include "imgui.h"                      //  0.3     "DEAR IMGUI" HEADERS...
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //
#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers



namespace cb { namespace utl { //     BEGINNING NAMESPACE "cb" :: "utl"...
// *************************************************************************** //
// *************************************************************************** //




//  0.1     INLINE DEFINITIONS (HEADER ONLY)        |   WIDGETS & WINDOWS...
// *************************************************************************** //
// *************************************************************************** //

//  "file_exists"
//
inline bool file_exists(const char* path)
{
    namespace fs = std::filesystem;
    fs::path p{path};
    return fs::exists(p) && fs::is_regular_file(p);
}


//  "WindowLocation"
//
enum class WindowLocation {
    Center,             //  default.
    LeftHalf,           //  left half of the monitor.
    RightHalf,          //  right half of the monitor.
    Fill,               //  Expand window to fill the entire screen.
    COUNT
};


//  "Anchor"
//
enum class Anchor {
    Center,             //  default.
    East,
    NorthEast,
    North,
    NorthWest,
    West,
    SouthWest,
    South,
    SouthEast,
//
    COUNT
};



// *************************************************************************** //
//
//
//
//  0.2     INLINE                                  | MISC. STRUCTS...
// *************************************************************************** //
// *************************************************************************** //

//  "WidgetRow"
//      Declarative per-row widget logic
struct WidgetRow {
    const char *            label;
    std::function<void()>   render;
};


//  "ChannelIdx"
//
enum ChannelIdx : std::size_t {
    UNUSED = 0,
    D,  C,  CD,
    B,  BD, BC, BCD,
    A,  AD, AC, ACD,
    AB, ABD, ABC, ABCD,
    CHANNEL_COUNT   // = 16
};


//  "CoincidencePacket"
//      Holds a single FPGA “packet”
struct CoincidencePacket
{
    std::array<int, CHANNEL_COUNT> counts{};
    int cycles{};

    //  Convenience accessors
    int d           (void) const    { return counts[D];  }
    int c           (void) const    { return counts[C];  }
    int cd          (void) const    { return counts[CD]; }
    int b           (void) const    { return counts[B];  }
    int bd          (void) const    { return counts[BD]; }
    int bc          (void) const    { return counts[BC]; }
    int bcd         (void) const    { return counts[BCD];}
    int a           (void) const    { return counts[A];  }
    int ad          (void) const    { return counts[AD]; }
    int ac          (void) const    { return counts[AC]; }
    int acd         (void) const    { return counts[ACD];}
    int ab          (void) const    { return counts[AB]; }
    int abd         (void) const    { return counts[ABD];}
    int abc         (void) const    { return counts[ABC];}
    int abcd        (void) const    { return counts[ABCD];}
};


//  "parse_packet"
//      Parse one JSON‑line; returns nullopt on format errors
inline std::optional<CoincidencePacket>
parse_packet(std::string_view line)
{
    using json = nlohmann::json;
    CoincidencePacket pkt;

    try {
        json j = json::parse(line);
        const auto& arr = j.at("counts");
        if (arr.size() != CHANNEL_COUNT)         // sanity check
            return std::nullopt;

        for (std::size_t i = 0; i < CHANNEL_COUNT; ++i)
            pkt.counts[i] = arr[i].get<int>();

        pkt.cycles = j.at("cycles").get<int>();
        return pkt;

    } catch (const json::exception&) {
        return std::nullopt;                     // malformed JSON / keys
    }
}



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
const char *                    get_glfw_version            (void);



//      1.2     WINDOW / GLFW FUNCTIONS...
// *************************************************************************** //
void                            glfw_error_callback         (int error, const char * description);
[[nodiscard]] GLFWwindow *      CreateGLFWWindow            (int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share);



//      1.3     WINDOW SIZE / GEOMETRY FUNCTIONS...
// *************************************************************************** //
[[nodiscard]]
std::pair<int, int>             GetMonitorDimensions        (GLFWwindow * window);
[[nodiscard]] float             GetDPIScaling               (GLFWwindow * window);

[[nodiscard]]
GLFWmonitor *                   get_current_monitor         (GLFWwindow * window);
void                            set_window_scale            (GLFWwindow * window, const float scale);
void                            set_next_window_geometry    (GLFWwindow * glfw_window, float pos_x_frac, float pos_y_frac,
                                                                                       float width_frac, float height_frac);

void                            SetGLFWWindowLocation       (GLFWwindow *   win, const      WindowLocation          loc,
                                                            const float     scale=0.5f,     const GLFWmonitor *     monitor=nullptr);
ImVec2                          GetImGuiWindowCoords        (const char * , const Anchor & );


//      1.4     WIDGET FUNCTIONS...
// *************************************************************************** //
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
                                //  1.6A-1      SAVING/WRITING FUNCTIONS...
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  const std::string &         file_path);
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  const char *                file_path);
bool                            SaveStyleToDisk             (const ImGuiStyle &     style,  std::string_view            file_path);

                                //  1.6A-2      ASYNCHRONUS SAVING/WRITING FUNCTIONS...
bool                            SaveStyleToDiskAsync        (const ImGuiStyle &     style,  const char *                file_path);
bool                            SaveStyleToDiskAsync        (ImGuiStyle             style,  const char *                file_path);


                                //  1.6B-1      LOADING FUNCTIONS...
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const char *                file_path);
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const std::string &         file_path);
bool                            LoadStyleFromDisk           (ImGuiStyle &           style,  const std::string_view &    file_path);

                                //  1.6B-2      ASYNCHRONUS LOADING FUNCTIONS...


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

//      2.1     OLD PLOTTING STUFF...
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
