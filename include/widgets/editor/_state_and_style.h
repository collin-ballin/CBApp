/***********************************************************************************
*
*       ********************************************************************
*       ****     _ S T A T E _ A N D _ S T Y L E . H  ____  F I L E     ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      September 19, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_STATE_AND_STYLE_H
#define _CBWIDGETS_EDITOR_STATE_AND_STYLE_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/editor/_constants.h"
//
//
//  #ifndef _CBAPP_EDITOR_OBJECTS_H
//  # include "widgets/editor/objects/objects.h"
//  #endif //  _CBAPP_EDITOR_OBJECTS_H  //
//
//
#ifndef _CBWIDGETS_EDITOR_TYPES_H
# include "widgets/editor/_types.h"
#endif //  _CBWIDGETS_EDITOR_TYPES_H  //



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <tuple>
#include <utility>

#include <algorithm>
#include <array>
#include <unordered_set>
#include <optional>

#include <string>           //  <======| std::string, ...
#include <string_view>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>

//  0.3     "DEAR IMGUI" HEADERS...
#include "json.hpp"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //






// *************************************************************************** //
//
//
//      1.      MISC. / DEPRECATED STUFF...
// *************************************************************************** //
// *************************************************************************** //

//  "Bounds"
//
struct Bounds {
    float min_x{0}, min_y{0}, max_x{0}, max_y{0};
};


//  "Camera"
//
struct Camera {
    ImVec2  pan             {0,0};      // world coords of viewport top-left
    float   zoom_mag        = 1.0f;     // 1 = fit, >1 zooms in, <1 zooms out

    // helpers --------------------------------------------------------
    inline float pixels_per_world(const ImVec2& view_sz, const Bounds& world) const {
        float w = world.max_x - world.min_x;
        float h = world.max_y - world.min_y;
        if (w <= 0) w = 1;              // avoid div-by-zero
        if (h <= 0) h = 1;
        float fit = std::min(view_sz.x / w, view_sz.y / h);
        return fit * zoom_mag;
    }
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "DEPRECATED" STUFF.











// *************************************************************************** //
//
//
//
//      3.      OTHER...
// *************************************************************************** //
// *************************************************************************** //

//  "ZOrderCFG_t"
//
template<typename ZID>
struct ZOrderCFG_t {
    ZID         Z_EDITOR_BACK               = 1;                        //  Grid / background
    ZID         Z_FLOOR_USER                = 255;                      //  First user layer
    ZID         Z_EDITOR_FRONT              = UINT32_MAX - 2;           //  Overlays, guides
    ZID         Z_CEIL_USER                 = Z_EDITOR_FRONT - 1;       //  Max allowed for user items
    ZID         RENORM_THRESHOLD            = 10'000;                   //  Span triggering re-pack
};


//  "to_json"
//
template <typename ZID>
inline void to_json(nlohmann::json & j, const ZOrderCFG_t<ZID> & obj) {
    j = {
        { "Z_EDITOR_BACK",          obj.Z_EDITOR_BACK       },
        { "Z_FLOOR_USER",           obj.Z_FLOOR_USER        },
        { "Z_EDITOR_FRONT",         obj.Z_EDITOR_FRONT      },
        { "Z_CEIL_USER",            obj.Z_CEIL_USER         },
        { "RENORM_THRESHOLD",       obj.RENORM_THRESHOLD    }
    };
    
    return;
}


//  "from_json"
//
template <typename ZID>
inline void from_json(nlohmann::json & j, ZOrderCFG_t<ZID> & obj)
{
    j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK       );
    j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER        );
    j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT      );
    j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER         );
    j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD    );
    
    return;
}



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "4.  REMAINDER".












// *************************************************************************** //
//
//
//
//      2.      "BROWSER" STATE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      2A. BROWSER STATES |        BROWSER---STATE.
// *************************************************************************** //

//  "BrowserState_t"
//
template<ObjectCFGTraits CFG>
struct BrowserState_t
{
// *************************************************************************** //
// *************************************************************************** //


    // *************************************************************************** //
    //      1.      NESTED TYPENAME ALIASES...
    // *************************************************************************** //
    //  USE_OBJECT_CFG_ALIASES(CFG);
    _USE_OBJECT_CFG_ALIASES
    //
    using                       Vertex                                  = Vertex_t      <CFG>                               ;
    using                       Path                                    = Path_t        <CFG, Vertex>                       ;

    // *************************************************************************** //
    //      2.      CONSTEXPR VALUES...
    // *************************************************************************** //
    static constexpr size_t     ms_MAX_PATH_TITLE_LENGTH                = Path::ms_MAX_PATH_LABEL_LENGTH + 64ULL;
    //
    static constexpr size_t     ms_MAX_VERTEX_TITLE_LENGTH              = Vertex::ms_VERTEX_NAME_BUFFER_SIZE + 32ULL;

    // *************************************************************************** //
    //      3.      MEMBER FUNCTIONS...
    // *************************************************************************** //
    
    //  "reset"
    inline void                 reset(void) { this->clear(); }
    
    //  "clear"
    inline void                 clear(void)
    {
        //  Renaming "Active" State.
        m_renaming_layer            = false;
        m_renaming_obj              = false;


        //  Force filter to rebuild so the clipper sees an empty list.
        m_layer_filter              .Build();
        m_obj_filter                .Build();
        
        
        //  NEW STUFF...
        //
        //      (A)     LAYER.
            //  m_layer_rows                .clear();
            //  m_renaming_layer            = false;            //  "Dirty" Flags (to rebuild list of indices).
            //  m_layer_rows_paths_rev      = -1;
        //
        //      (B)     OBJECT.
        m_obj_rows                  .clear();
        m_renaming_obj              = false;            //  "Dirty" Flags (to rebuild list of indices).
        m_obj_rows_paths_rev        = -1;
        
        
        
        //  Rename Index.
        m_layer_rename_idx          = -1;
        m_obj_rename_idx            = -1;
        
        
        
        //  Hovered OBJECT in Browser.
        m_hovered_obj               = -1;           //  Hovered OBJECT in Browser-Selectable.
        m_hovered_canvas_obj        = -1;           //  Hovered OBJECT in Browser-Canvas.
        
        //  Hovered VERTEX in Browser.
        m_hovered_vertex            = { -1, -1 };   //  Hovered VERTEX in Browser.
        
          
          
          
        //  ??  Other  ??
        m_layer_browser_anchor      = -1;
        m_browser_anchor            = -1;
        m_inspector_vertex_idx      = -1;
        
        return;
    }
    
    // *************************************************************************** //

    // *************************************************************************** //
    //      3.1.    UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    //  "HasAuxiliarySelection"
    [[nodiscard]] inline bool           HasAuxiliarySelection           (void) const noexcept {
        const bool obj_1    = !(this->m_hovered_obj    < 0);
        const bool obj_2    = ( !(this->m_hovered_vertex.first < 0)  &&  !(this->m_hovered_vertex.second < 0) );
        
        return ( obj_1  ||  obj_2 );
    }
    
    //  "ClearAuxiliarySelection"
    inline void                         ClearAuxiliarySelection         (void) const noexcept {
        this->m_hovered_obj             = -1;
        this->m_hovered_canvas_obj      = -1;
        
        this->m_hovered_vertex          = { -1, -1 };
        return;
    }
    
    
    
    // *************************************************************************** //


    // *************************************************************************** //
    //      3.      DATA MEMBERS...
    // *************************************************************************** //
    //                      MUTABLE STATE VARIABLES:
    bool                        m_show_default_properties                   = false;
    bool                        m_renaming_layer                            = false;    //  TRUE when user is in the middle of MODIFYING NAME.
    bool                        m_renaming_obj                              = false;    //
    //
    //
    //                      NEW STUFF:
        //      std::vector<int>            m_layer_rows                                {  };
        //      bool                        m_layer_filter_dirty                        = false;    //  Flag to queue Browser to RE-COMPUTE sorted items.
        //      int                         m_layer_rows_paths_rev                      = -1;
    //
    std::vector<int>            m_obj_rows                                  {   };
    bool                        m_obj_filter_dirty                          = false;    //
    int                         m_obj_rows_paths_rev                        = -1;
    //
    //
    //                      INDICES:
    int                         m_layer_browser_anchor                      = -1;       //  ?? currently selected LAYER ??
    //
    //
    int                         m_browser_anchor                            = -1;       //  ?? anchor index for Shift‑range select ??
    //
    mutable int                 m_hovered_obj                               = -1;
    mutable int                 m_hovered_canvas_obj                        = -1;
    //
    //
    int                         m_inspector_vertex_idx                      = -1;       //  ...
    mutable std::pair<int,int>  m_hovered_vertex                            = {-1, -1};
    //
    //
    int                         m_layer_rename_idx                          = -1;       //  LAYER that is BEING RENAMED     (–1 = none)
    int                         m_obj_rename_idx                            = -1;       //  OBJECT that is BEING RENAMED    (–1 = none)
    //
    //
    //                      CACHE AND MISC. DATA:
    char                        m_name_buffer[ ms_MAX_PATH_TITLE_LENGTH ]   = {   };    //  scratch text
    //
    //
    //
    //                      OTHER DATA ITEMS:
    ImGuiTextFilter             m_layer_filter;                                         //  search box for "LAYER" browser.
    ImGuiTextFilter             m_obj_filter;                                           //  search box for "OBJECT" browser.
    
    
    
    // *************************************************************************** //

// *************************************************************************** //
// *************************************************************************** //   END "BrowserState_t".
};






// *************************************************************************** //
//      2B. BROWSER STYLE |         BROWSER---STYLE.
// *************************************************************************** //

//  "BrowserStyle"
//
struct BrowserStyle
{
    CBAPP_CBLIB_TYPES_API
    
// *************************************************************************** //
//                                      BROWSER WINDOW STYLE...
// *************************************************************************** //
    //
    //                              BROWSER CHILD-WINDOW FLAGS:
    ImGuiChildFlags                     DYNAMIC_CHILD_FLAGS                         = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    ImGuiChildFlags                     STATIC_CHILD_FLAGS                          = ImGuiChildFlags_Borders;
    //
    //                              BROWSER CHILD-WINDOW DIMENSIONS:
    float                               ms_CHILD_WINDOW_SAMELINE                    = 4.0f;
    //
    Param<ImVec2>                       TRAIT_SELECTOR_DIMS                         = {     {120.0f,    -1.0f},     { {80.0f,       1.0f},      {220.0f,    FLT_MAX} }   };
    //
    Param<ImVec2>                       OBJ_SELECTOR_DIMS                           = {     {300.0f,    -1.0f},     { {250.0f,      1.0f},      {450.0f,    FLT_MAX} }   };
    Param<ImVec2>                       OBJ_PROPERTIES_INSPECTOR_DIMS               = {     {650.0f,    -1.0f},     { {600.0f,      1.0f},      {1200.0f,   FLT_MAX} }   };
    Param<ImVec2>                       VERTEX_SELECTOR_DIMS                        = {     {110.0f,    -1.0f},     { {90.0f,       1.0f},      {180.0f,    FLT_MAX} }   };
    //
    //
    //
    //                              BROWSER CHILD-WINDOW COLORS:
    ImVec4                              ms_TRAIT_INSPECTOR_FRAME_BG                 = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);
    //
    ImVec4                              ms_VERTEX_SELECTOR_FRAME_BG                 = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);
    ImVec4                              ms_VERTEX_INSPECTOR_FRAME_BG                = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);
    //
    //
    //
    //
    //
    ImVec4                              ms_CHILD_FRAME_BG1                          = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    ImVec4                              ms_CHILD_FRAME_BG1L                         = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1L     //   #17191E
    ImVec4                              ms_CHILD_FRAME_BG1R                         = ImVec4(0.141f,    0.141f,     0.141f,     1.000f); // ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1R     //   #21242B
    
    ImVec4                              ms_CHILD_FRAME_BG2                          = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//      ms_CHILD_FRAME_BG2      // BASE = #52596B
    ImVec4                              ms_CHILD_FRAME_BG2L                         = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//      ms_CHILD_FRAME_BG2L     // ##353A46
    ImVec4                              ms_CHILD_FRAME_BG2R                         = ImVec4(0.129f,    0.140f,     0.168f,     0.800f); // ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//      ms_CHILD_FRAME_BG2R     // #5B6377
    //
    //                              NEW COLORS:
    ImVec4                              ms_OBJ_INSPECTOR_FRAME_BG                   = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    //
    //
    //                              BROWSER CHILD-WINDOW STYLE:
    float                               ms_VERTEX_SUBBROWSER_HEIGHT                 = 0.85f;    //  ms_VERTEX_SUBBROWSER_HEIGHT
    float                               ms_CHILD_BORDER1                            = 2.0f;     //  ms_CHILD_BORDER1
    float                               ms_CHILD_BORDER2                            = 1.0f;     //  ms_CHILD_BORDER2
    float                               ms_CHILD_ROUND1                             = 8.0f;     //  ms_CHILD_ROUND1
    float                               ms_CHILD_ROUND2                             = 4.0f;     //  ms_CHILD_ROUND2
    //
    //                              BROWSER CHILD-WINDOW DIMENSIONS:
    float                               OBJ_PROPERTIES_REL_WIDTH                    = 0.5f;     // Relative width of OBJECT PROPERTIES PANEL.
    float                               VERTEX_SELECTOR_REL_WIDTH                   = 0.075f;   // Rel. width of Vertex SELECTOR COLUMN.
    float                               VERTEX_INSPECTOR_REL_WIDTH                  = 0.0f;     // Rel. width of Vertex INSPECTOR COLUMN.

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      GENERAL WIDGET STUFF...
// *************************************************************************** //
    //                              BROWSER **ALL** WIDGET STUFF:
    //  float                               ms_BROWSER_BUTTON_SEP                       = 8.0f;
    //  float                               ms_BROWSER_SELECTABLE_SEP                   = 16.0f;
    std::pair<float,float>              ms_BROWSER_ITEM_SEPS                        = { 8.0f,   16.0f };
    //
    //                              BROWSER PATH WIDGET STUFF:
    float                               ms_BROWSER_OBJ_LABEL_WIDTH                  = 55.0f;
    float                               ms_BROWSER_OBJ_WIDGET_WIDTH                 = 256.0f;
    //
    //                              BROWSER VERTEX WIDGET STUFF:
    float                               ms_BROWSER_VERTEX_LABEL_WIDTH               = 75.0f;
    float                               ms_BROWSER_VERTEX_WIDGET_WIDTH              = 196.0f;

// *************************************************************************** //
//
//
//
// *************************************************************************** //
//                                      SPECIFIC WIDGET STUFF...
// *************************************************************************** //
    //                              DELETE BUTTON:
    static constexpr const char *       ms_DELETE_BUTTON_HANDLE                     = "x";
    static constexpr ImU32              ms_DELETE_BUTTON_COLOR                      = cblib::utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   1.000f,     0.271f,     0.227f,     0.500f      )  );
    static constexpr size_t             ms_ACTION_DESCRIPTION_LIMIT                 = 256ULL;
    //
    //                              DRAG/DROP STUFF:
    static constexpr const char *       ms_DRAG_HANDLE_ICON                         = "=";

//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA MEMBERS".






// *************************************************************************** //
//
//
//      2.C         INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      1.      UTILITY FUNCTIONS...
    // *************************************************************************** //
    
    //
    //  ...
    //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.
    


// *************************************************************************** //
// *************************************************************************** //   END "EditorBrowserStyle"
};



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "3.  BROWSER STATE".













// *************************************************************************** //
//
//
//
//      4.      "EDITOR" STATE...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
//      4A. EDITOR STATES |         EDITOR---RUNTIME.
// *************************************************************************** //

//  "EditorRuntime_t"
//      Contains the NON-COPYABLE, NON-MOVABLE DATA that pertains to the Editor State.
//
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
struct EditorRuntime_t
{
    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    
    // *************************************************************************** //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    
    // *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//      1.          DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    std::mutex                              m_task_mtx                      {   };
    std::vector< std::function<void()> >    m_main_tasks                    {   };
    
    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    std::atomic<bool>                       m_io_busy                       { false };
    std::atomic<bool>                       m_sdialog_open                  { false };
    std::atomic<bool>                       m_odialog_open                  { false };
    std::atomic<bool>                       m_show_io_message               { false };
    
//
// *************************************************************************** //
// *************************************************************************** //   END "DATA-MEMBERS".



// *************************************************************************** //
//
//      2.A.        MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        EditorRuntime_t             (void) noexcept                     = default;
                                        ~EditorRuntime_t            (void)                              = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        EditorRuntime_t             (const EditorRuntime_t &    src)    = delete;   //  Copy. Constructor.
                                        EditorRuntime_t             (EditorRuntime_t &&         src)    = delete;   //  Move Constructor.
    EditorRuntime_t &                   operator =                  (const EditorRuntime_t &    src)    = delete;   //  Assgn. Operator.
    EditorRuntime_t &                   operator =                  (EditorRuntime_t &&         src)    = delete;   //  Move-Assgn. Operator.
    
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.

//
//
// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorRuntime" INLINE STRUCT DEFINITION.






// *************************************************************************** //
//      4B. EDITOR STATES |         EDITOR---STATE.
// *************************************************************************** //

//  "EditorState_t"
//
template< typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HID >
struct EditorState_t
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float                  ms_IO_MESSAGE_DURATION          = 5.0f;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    //                              //  ...
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //                  OVERALL STATE...
    // *************************************************************************** //
    ImPlotInputMap                          m_backup;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  PERSISTENT STATE INFORMATION...
    // *************************************************************************** //
    //                                  OVERALL STATE / ENABLED BEHAVIORS:
    mutable bool                            m_block_overlays                = false;
    //
    bool                                    m_show_debug_overlay            = true;     //  Persistent UI Resident Overlays.
    bool                                    m_show_ui_traits_overlay        = true;
    bool                                    m_show_ui_objects_overlay       = true;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  IMPLOT CANVAS INFORMATION...
    // *************************************************************************** //
    //                                  CONSTANTS:
    static constexpr double                 ms_INITIAL_CANVAS_SIZE [4]          = { 0.0f, 256.0f, 0.0f, 256.0f };
    static constexpr double                 ms_INPUT_DOUBLE_INCREMENTS [2]      = { 1.0f, 10.0f };                      //  Snap value of "+" and "-" BUTTONS.
    //
    //
    //                                  PERSISTENT STATE INFORMATION:
    std::array< Param<double>, 2>           m_world_size                        = { {                                   //  MAXIMUM SIZE OF THE CANVAS (World Size).
                                                                                    { 512.0f,       { 10.0f,        1e4f } },
                                                                                    { 512.0f,       { 10.0f,        1e4f } }
                                                                                } };
    std::array< Param<double>, 2>           m_world_slop                        = { {                                   //  (Allow user to scroll a bit beyond canvas limits).
                                                                                    { 128.0f,       { 32.0f,        512.0f } },
                                                                                    { 128.0f,       { 32.0f,        512.0f } }
                                                                                } };
    std::array< Param<double>, 2>           m_zoom_size                         = { {                                   //  MAX + MIN "ZOOM" RESOLUTION OF THE CANVAS.
                                                                                    { 1024.0f,      { 1.0f,         2e4f } },
                                                                                    { 1024.0f,      { 1.0f,         2e4f } }
                                                                                } };
    //
    //                                  GRID INFORMATION:
    std::array< Param<int>, 2>              m_grid_density                      = { {                                   //  TOTAL # OF GRIDLINES.
                                                                                    { 16,           { 2,            64 } },
                                                                                    { 16,           { 2,            64 } }
                                                                                } };
    std::array< std::vector<double>, 2 >    m_gridlines;                                                            //  POSITION OF THE "X" AND "Y" GRIDLINES.
    std::array< double, 2 >                 m_grid_spacing                      = { -1.0f,      -1.0f };                //  DIST. BETWEEN EACH GRIDLINE.
    
    
    //
    //
    //                                  TRANSIENT STATE INFORMATION:
    //  mutable bool                            m_request_canvas_window_update      = true;
    mutable ImPlotRect                      m_window_coords                     = {   };        //  DOMAIN + RANGE OF CURRENT CANVAS:   ( [X0, Xf], [Y0, Yf] ).
    std::array< double, 2>                  m_window_size                       = {   };
    //
    mutable ImVec2                          m_plot_px_dims                      = {   };
    mutable ImRect                          m_plot_bbox                         = {   };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  IMPLOT SETTINGS INFORMATION...
    // *************************************************************************** //
    //                                  PERSISTENT STATE INFORMATION:
    Param<float>                            m_mousewheel_zoom_rate              = { 0.050f,     { 0.010f,   0.350f } };
    //
    //
    //                                  TRANSIENT STATE INFORMATION:
    //                                      ...
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  TRANSIENT STATE INFORMATION...
    // *************************************************************************** //
    //                                  UTILITY:
    float                                   m_bar_h                             = 0.0f;
    ImVec2                                  m_avail                             = ImVec2(0.0f,      0.0f);
    ImVec2                                  m_p0                                = ImVec2(0.0f,      0.0f);
    ImVec2                                  m_p1                                = ImVec2(0.0f,      0.0f);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  TODO: RE-HOME THESE INTO TOOL STATE OBJs...
    // *************************************************************************** //
    //
    //                                  LASSO TOOL / SELECTION:
    bool                                    m_show_sel_overlay                  = false;
    //
    bool                                    m_dragging                          = false;
    bool                                    m_lasso_active                      = false;
    bool                                    m_pending_clear                     = false;    //  pending click selection state ---
    //
    ImVec2                                  m_lasso_start                       = ImVec2(0.f,       0.f);
    ImVec2                                  m_lasso_end                         = ImVec2(0.f,       0.f);
    VID                                     m_next_id                           = 1;
    PID                                     m_next_pid                          = 1;        // counter for new path IDs
    //
    //
    //                                  PEN-TOOL STATE:
    bool                                    m_drawing                           = false;
    bool                                    m_dragging_handle                   = false;
    bool                                    m_dragging_out                      = true;
    VID                                     m_drag_vid                          = 0;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                  MISC STATE...
    // *************************************************************************** //

                                            //
                                            //  ...
                                            //

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //                  SERIALIZATION STUFF...
    // *************************************************************************** //
    std::mutex                              m_task_mtx;
    std::vector<std::function<void()>>      m_main_tasks;
    //
    std::atomic<bool>                       m_io_busy                       { false };
    IOResult                                m_io_last                       { IOResult::Ok };
    std::string                             m_io_msg                        {   };
    //
    std::atomic<bool>                       m_sdialog_open                  = { false };
    std::atomic<bool>                       m_odialog_open                  = { false };
    std::string                             m_project_name                  = {   };
    std::filesystem::path                   m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-default.json"};   //    {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    //      std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/presets/editor/testing/editor-fdtd_v0.json"};
    //
    //
    //                                  PEN-TOOL STATE:
    std::atomic<bool>                       m_show_io_message               { false };
    float                                   m_io_message_timer              = -1.0f;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//      2.A.        PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    //  explicit                        MyClass                 (app::AppState & );             //  Def. Constructor.
                                        EditorState_t           (void) noexcept
    {
        //      1.      ALLOCATE MEMORY FOR GRIDLINES...
        this->m_gridlines[0].reserve( this->m_grid_density[0].Max() );      //  X-Axes.
        this->m_gridlines[1].reserve( this->m_grid_density[1].Max() );      //  Y-Axes.
        this->_update_grid();
        
        
        return;
    }
                                        //  EditorState_t           (void) noexcept                 = default;
                                        ~EditorState_t          (void)                          = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
                                        EditorState_t           (const EditorState_t &    src)  = default;      //  Copy. Constructor.
                                        EditorState_t           (EditorState_t &&         src)  = default;      //  Move Constructor.
    EditorState_t &                     operator =              (const EditorState_t &    src)  = default;      //  Assgn. Operator.
    EditorState_t &                     operator =              (EditorState_t &&         src)  = default;      //  Move-Assgn. Operator.
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".


   
// *************************************************************************** //
//
//
//      2.B.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      MAIN FUNCTIONS...
    // *************************************************************************** //
    
    //  "DisplayIOStatus"
    inline void                         DisplayIOStatus                     (void) noexcept
    {
        ImGuiIO &           io                  = ImGui::GetIO();
        this->m_io_message_timer               -= io.DeltaTime;
        
        
        //      1.      DECREMENT TIMER...
        if ( this->m_io_message_timer <= 0.0f ) {
            this->m_io_message_timer = -1.0f;
            this->m_show_io_message.store(false, std::memory_order_release);
            return;
        }
        //
        //      2.      DISPLAY TOOL-TIP I/O MESSAGING...
        //  ImGui::SetNextWindowBgAlpha(0.75f); // optional: semi-transparent
        ImGui::BeginTooltip();
            ImGui::TextUnformatted( this->m_io_msg.c_str() );
        ImGui::EndTooltip();
    
        return;
    }
    
    

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS.
    // *************************************************************************** //
    
    //  "CanDecreaseGridSpacing"
    [[nodiscard]] inline bool           CanDecreaseGridSpacing              (void) const noexcept
    {
        const bool  decr_x  = ( this->m_grid_density[0].CanDecrement() );
        const bool  decr_y  = ( this->m_grid_density[1].CanDecrement() );
        return ( decr_x  &&  decr_y);
    }
    
    //  "CanIncreaseGridSpacing"
    [[nodiscard]] inline bool           CanIncreaseGridSpacing              (void) const noexcept
    {
        const bool  incr_x  = ( this->m_grid_density[0].CanIncrement() );
        const bool  incr_y  = ( this->m_grid_density[1].CanIncrement() );
        return ( incr_x  &&  incr_y);
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "SetLastIOStatus"
    inline void                         SetLastIOStatus                     (const IOResult result, std::string & message) noexcept
    {
        this->m_io_busy                 .store( false,   std::memory_order_release   );
        this->m_show_io_message         .store( true,    std::memory_order_release   );
        this->m_io_message_timer        = ms_IO_MESSAGE_DURATION;
        //
        this->m_io_last                 = result;
        this->m_io_msg                  = std::move( message );
        
        return;
    }
    
    
    //  "UpdateCanvasSize"
    inline void                         UpdateCanvasSize                    (void) noexcept
    {
        return;
    }
    
    //  "SetupImPlotGrid"
    inline void                         SetupImPlotGrid                     (void) const noexcept
    {
        ImPlot::SetupAxisTicks(   ImAxis_X1
                                , this->m_gridlines[0].data()
                                , static_cast<int>( this->m_gridlines[0].size() )
                                , /*labels        */nullptr
                                , /*show_default  */false );
        ImPlot::SetupAxisTicks( ImAxis_Y1
                                , this->m_gridlines[1].data()
                                , static_cast<int>( this->m_gridlines[1].size() )
                                , /*labels        */nullptr
                                , /*show_default  */false );
        return;
    }

    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      IMPLOT GRID MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "IncreaseGridSpacing"
    inline bool                         IncreaseGridSpacing                 (void) noexcept {
        if ( !this->CanIncreaseGridSpacing() )      { return false; }
    
        this->m_grid_density[0].SetValue            ( this->m_grid_density[0].value << 1 );
        this->m_grid_density[1].SetValue            ( this->m_grid_density[1].value << 1 );
        _update_grid();
        return true;
    }
    inline void                         IncreaseGridSpacingX                (void) noexcept {
        static_assert( true, "Not implemented" );
        //  this->m_grid_density[0].SetValue( this->m_grid_density[0].value << 1 );
        //  _update_grid_x();
        return;
    }
    inline void                         IncreaseGridSpacingY                (void) noexcept {
        static_assert( true, "Not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[1].value << 1 );
        //  this->_update_grid_y();
        return;
    }
    
    
    //  "DecreaseGridSpacing"
    inline bool                         DecreaseGridSpacing                 (void) noexcept {
        if ( !this->CanDecreaseGridSpacing() )      { return false; }
        
        this->m_grid_density[0].SetValue            ( this->m_grid_density[0].value >> 1 );
        this->m_grid_density[1].SetValue            ( this->m_grid_density[1].value >> 1 );
        _update_grid();
        return true;
    }
    inline void                         DecreaseGridSpacingX                (void) noexcept {
        static_assert( true, "Not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[0].value >> 1 );
        //  this->_update_grid_x();
        return;
    }
    inline void                         DecreaseGridSpacingY                (void) noexcept {
        static_assert( true, "Not implemented" );
        //  this->m_grid_density[1].SetValue( this->m_grid_density[1].value >> 1 );
        //  this->_update_grid_y();
        return;
    }
    
    
    //  "_compute_grid_spacing"
    inline void                         _compute_grid_spacing               (void) noexcept
    {
        return;
    }
    
    
    //  "_update_grid"
    inline void                         _update_grid                        (void) noexcept
    {
        const size_t        NX      = static_cast<size_t>( this->m_grid_density[0].Value() );
        const size_t        NY      = static_cast<size_t>( this->m_grid_density[1].Value() );
        const double        xmax    = this->m_world_size[0].Value();
        const double        ymax    = this->m_world_size[1].Value();
        
            
        //      1.      UPDATE DIST. BETWEEN EACH GRIDLINE...
        m_grid_spacing[0]           = xmax / static_cast<double>( NX );
        m_grid_spacing[1]           = ymax / static_cast<double>( NY );
        
        
        //      2.      RE-SIZE ARRAYS OF GRIDLINE POSITIONS...
        this->m_gridlines[0]        .resize(NX);
        this->m_gridlines[1]        .resize(NY);
        
        
        //      3.      POPULATE ARRAYS WITH NEW VALUES...
        //
        //              3A.     X-AXIS.
        for (size_t x = 0; x < NX; ++x)
        {
            this->m_gridlines[0][x]     = static_cast<double>( x ) * m_grid_spacing[0];
        }
        //              3B.     Y-AXIS.
        for (size_t y = 0; y < NY; ++y)
        {
            this->m_gridlines[1][y]     = static_cast<double>( y ) * m_grid_spacing[1];
        }
    
        return;
    }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "EditorState"
};



//      EditorState     : "to_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void to_json(nlohmann::json & j, const EditorState_t<VID, PtID, LID, PID, ZID, HitID> & obj)
{
    j ["m_world_size"]      = obj.m_world_size;
    j ["m_world_slop"]      = obj.m_world_slop;
    j ["m_zoom_size"]       = obj.m_zoom_size;

    //  j = {
    //      { "m_world_size",          obj.m_world_size       }
    //  };
    
    /*
    Param<double>                           m_world_size [2]                = {                                     //  MAXIMUM SIZE OF THE CANVAS (World Size).
                                                                                { 512.0f,       { 10.0f,        1e4f } },
                                                                                { 512.0f,       { 10.0f,        1e4f } }
                                                                            };
    Param<double>                           m_world_slop [2]                = {                                     //  (Allow user to scroll a bit beyond canvas limits).
                                                                                { 128.0f,       { 32.0f,        512.0f } },
                                                                                { 128.0f,       { 32.0f,        512.0f } }
                                                                            };
    Param<double>                           m_zoom_size [2]                 = {                                     //  MAX + MIN "ZOOM" RESOLUTION OF THE CANVAS.
                                                                                { 1024.0f,      { 1.0f,         2e4f } },
                                                                                { 1024.0f,      { 1.0f,         2e4f } }
              */
    
    
    return;
}


//      EditorState     : "from_json"
//
template<typename VID, typename PtID, typename LID, typename PID, typename ZID, typename HitID>
inline void from_json([[maybe_unused]] nlohmann::json & j, [[maybe_unused]] EditorState_t<VID, PtID, LID, PID, ZID, HitID> & obj)
{

    //  j.at("")        .get_to(obj.m_plot_limits);

    //  j.at("Z_EDITOR_BACK")           .get_to(obj.Z_EDITOR_BACK);
    //  j.at("Z_FLOOR_USER")            .get_to(obj.Z_FLOOR_USER);
    //  j.at("Z_EDITOR_FRONT")          .get_to(obj.Z_EDITOR_FRONT);
    //  j.at("Z_CEIL_USER")             .get_to(obj.Z_CEIL_USER);
    //  j.at("RENORM_THRESHOLD")        .get_to(obj.RENORM_THRESHOLD);
    
    return;
}






// *************************************************************************** //
//      4C. EDITOR STYLE |         EDITOR---STYLE.
// *************************************************************************** //

//  "EditorStyle"
//
struct EditorStyle
{
// *************************************************************************** //
//                              STATIC CONSTEXPR CONSTANTS...
// *************************************************************************** //
    static constexpr size_t             DEF_HISTORY_CAP                             = 64;
    
// *************************************************************************** //
//                              SUBSIDIARY STYLE OBJECTS...
// *************************************************************************** //
    BrowserStyle                        browser_style                               = { };
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              UI-APPEARANCE CONSTANTS...
// *************************************************************************** //
    float                       ms_TOOLBAR_ICON_SCALE           = 1.5f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              INTERACTION / RESPONSIVENESS CONSTANTS...
// *************************************************************************** //
    float                       GRID_STEP                       = 64.0f;
    float                       HIT_THRESH_SQ                   = 6.0f * 6.0f;
    float                       DRAG_START_DIST_PX              = 4.0f;
//
    int                         PEN_DRAG_TIME_THRESHOLD         = 0.05;                             //  PEN_DRAG_TIME_THRESHOLD         // seconds.
    float                       PEN_DRAG_MOVEMENT_THRESHOLD     = 4.0f;                             //  PEN_DRAG_MOVEMENT_THRESHOLD     // px  (was 2)
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              STANDARDIZED STYLE...
// *************************************************************************** //
    ImU32                       COL_POINT_DEFAULT               = IM_COL32(0,255,0,255);        // idle green
    ImU32                       COL_POINT_HELD                  = IM_COL32(255,100,0,255);      // while dragging
    ImU32                       COL_SELECTION_OUT               = IM_COL32(255,215,0,170);      // gold outline
    float                       DEFAULT_POINT_RADIUS            = 12.0f;                        // px
    //
    //
    //  SEL BLUE    = IM_COL32(0,       180,    255,    153);
    //  GOLD        = IM_COL32(255,     215,    0,      127);
    //  GOOD BLUE   = IM_COL32(97,      145,    224,    170);
    //
    //
    ImU32                       HIGHLIGHT_COLOR                 = IM_COL32(0,       180,    255,    153);
    float                       HIGHLIGHT_WIDTH                 = 4.0f;
    ImU32                       AUX_HIGHLIGHT_COLOR             = IM_COL32(97,      145,    224,    170);
    float                       AUX_HIGHLIGHT_WIDTH             = 8.0f;
    //
    //
    //  ImU32                       AUX_HIGHLIGHT_COLOR             = IM_COL32(255,215,0,255);      // gold outline
    //  float                       AUX_HIGHLIGHT_WIDTH             = 6.0f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              CANVAS  | USER-INTERFACE CONSTANTS...
// *************************************************************************** //
//                          HANDLES:
    ImU32                       ms_HANDLE_COLOR                 = IM_COL32(255, 215, 0, 170);       //  ms_HANDLE_COLOR             //  gold    IM_COL32(255, 215, 0, 255)
    ImU32                       ms_HANDLE_HOVER_COLOR           = cblib::utl::compute_tint( IM_COL32(255, 215, 0, 255), 0.30f );       //  ms_HANDLE_HOVER_COLOR       //  yellow  IM_COL32(255, 255, 0, 255)
    float                       ms_HANDLE_SIZE                  = 5.5f;
    float                       HANDLE_BOX_SIZE                 = 5.5f;                              //  ms_HANDLE_SIZE              //  px half-side
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              CANVAS  | PREVIEW APPEARANCE CONSTANTS...
// *************************************************************************** //
//                          VERTICES:
    float                       VERTEX_PREVIEW_RADIUS           = 4.0f;
    ImU32                       VERTEX_PREVIEW_COLOR            = IM_COL32(255, 255, 0, 160);
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              TOOL CONSTANTS...
// *************************************************************************** //
//
//                          STANDARDIZED STYLE:
//
//                          PEN-TOOL ANCHORS:
//                              //  ...
//
//                          LASSO TOOL:
    ImU32                       COL_LASSO_OUT                   = IM_COL32(255,215,0,255); // gold outline
    ImU32                       COL_LASSO_FILL                  = IM_COL32(255,215,0,40);  // translucent fill
//
//                          BOUNDING BOX:
    float                       SELECTION_BBOX_MARGIN_PX        = 8.0f; //    12.0f;
    ImU32                       SELECTION_BBOX_COL              = IM_COL32(0, 180, 255, 153);   //  cyan-blue
    float                       SELECTION_BBOX_TH               = 2.5f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              CURSOR CONSTANTS...
// *************************************************************************** //
//                          PEN-TOOL CURSOR STUFF:
    float                       PEN_RING_RADIUS                 = 6.0f;                             //  PEN_RING_RADIUS     [px]
    float                       PEN_RING_THICK                  = 1.5f;                             //  PEN_RING_THICK      [px]
    int                         PEN_RING_SEGMENTS               = 32;                               //  NUMBER OF SEGMENTS TO DRAW OUTER RING.
    float                       PEN_DOT_RADIUS                  = 2.0f;                             //  PEN_DOT_RADIUS      // px
    int                         PEN_DOT_SEGMENTS                = 16;                               //  NUMBER OF SEGMENTS TO DRAW INNER DOT.
    //
    //
    ImU32                       PEN_COL_NORMAL                  = IM_COL32(255,255,0,255);          //  PEN_COL_NORMAL      // yellow
    ImU32                       PEN_COL_EXTEND                  = IM_COL32(  0,255,0,255);          //  PEN_COL_EXTEND      // green

// *************************************************************************** //
//
//
// *************************************************************************** //
//                              WIDGETS / UI-CONSTANTS / APP-APPEARANCE...
// *************************************************************************** //
    //                      SYSTEM PREFERENCES WIDGETS:
    float                       ms_SETTINGS_LABEL_WIDTH             = 196.0f;
    float                       ms_SETTINGS_WIDGET_AVAIL            = 256.0f;
    float                       ms_SETTINGS_INDENT_SPACING_CACHE    = 0.0f;
    float                       ms_SETTINGS_WIDGET_WIDTH            = -1.0f;
    float                       ms_SETTINGS_SMALL_LINE_HEIGHT       = -1.0f;
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              RENDERING CONSTANTS...
// *************************************************************************** //
    //                      VERTEX RENDERING:
    int                         ms_VERTEX_NUM_SEGMENTS          = 12;
    //
    //                      BEZIER RENDERING:
    int                         ms_BEZIER_SEGMENTS              = 0;        //  ms_BEZIER_SEGMENTS
    int                         ms_BEZIER_HIT_STEPS             = 20;       //  ms_BEZIER_HIT_STEPS
    int                         ms_BEZIER_FILL_STEPS            = 24;       //  ms_BEZIER_FILL_STEPS
    
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              UTILITY...
// *************************************************************************** //
                                //
                                //  ...
                                //
                                
// *************************************************************************** //
//
//
// *************************************************************************** //
//                              MISC. / UNKNOWN CONSTANTS (RELOCATED FROM CODE)...
// *************************************************************************** //
    float                       TARGET_PX                       = 20.0f;                    //     desired screen grid pitch"  | found in "_update_grid"
    float                       PICK_PX                         = 6.0f;                     //     desired screen grid pitch"  | found in "_update_grid"

// *************************************************************************** //

    
   
// *************************************************************************** //
//
//
//      2.C.        INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "PushSettingsWidgetW"
    inline void                         PushSettingsWidgetW                 (const uint8_t mult = 1) noexcept
    { this->ms_SETTINGS_WIDGET_WIDTH = this->ms_SETTINGS_WIDGET_AVAIL - static_cast<float>( mult ) * ms_SETTINGS_INDENT_SPACING_CACHE; }
    
    //  "PopSettingsWidgetW"
    inline void                         PopSettingsWidgetW                  (void) noexcept
    { this->ms_SETTINGS_WIDGET_WIDTH = this->ms_SETTINGS_WIDGET_AVAIL; }
    
    //  "SmallNewLine"
    inline void                         SmallNewLine                        (void) noexcept
    { ImGui::Dummy({ 0.0f, this->ms_SETTINGS_SMALL_LINE_HEIGHT}); return; }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "MyClass" INLINE CLASS DEFINITION.



//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "4.  EDITOR STYLE".


















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.












#endif      //  _CBWIDGETS_EDITOR_STATE_AND_STYLE_H  //
// *************************************************************************** //
// *************************************************************************** //   END FILE.
