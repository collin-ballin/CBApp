/***********************************************************************************
*
*       ********************************************************************
*       ****               E D I T O R . H  ____  F I L E               ****
*       ********************************************************************
*
*              AUTHOR:      Collin A. Bond.
*               DATED:      June 12, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_EDITOR_H
#define _CBWIDGETS_EDITOR_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
//  #include "app/_init.h"
#include "app/state/_init.h"
#include "app/state/state.h"

#include "utility/utility.h"
#include "widgets/editor/_icon.h"
#include "widgets/editor/_editor.h"
//  #include "widgets/editor/_types.h"

//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <iomanip>          //           for std::setw / std::setfill (ID labels)
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
#include "IconsFontAwesome6.h"
//
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"



namespace cb { //     BEGINNING NAMESPACE "cb"...
// *************************************************************************** //
// *************************************************************************** //



//      FORWARD DECLARATIONS...
// *************************************************************************** //
// *************************************************************************** //

namespace                       app                         { class     AppState;       }
namespace                       app                         { struct    MenuState_t;    }
//
struct                          EditorSnapshot;
class                           History;
struct                          SnapshotCmd;
struct                          Command;
//
//
struct                          Vertex_Tag                  {   };
struct                          Point_Tag                   {   };
struct                          Line_Tag                    {   };
struct                          Path_Tag                    {   };
struct                          Overlay_Tag                 {   };
struct                          Hit_Tag                     {   };



// *************************************************************************** //
// *************************************************************************** //
//                         Editor:
//                 Editor Widget for Dear ImGui.
// *************************************************************************** //
// *************************************************************************** //
  
//  "Editor"
//
class Editor
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    _EDITOR_APP_INTERNAL_API
    using                           MenuState                           = app::MenuState_t;
    //
    friend class                    App;
    friend struct                   EditorSnapshot;
    friend class                    History;
    friend struct                   SnapshotCmd;
    friend struct                   Command;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //                              VERSIONING:
    static constexpr auto               ms_MAJOR_VERSION                = EditorIMPL::ms_EDITOR_JSON_MAJ_VERSION;
    static constexpr auto               ms_MINOR_VERSION                = EditorIMPL::ms_EDITOR_JSON_MIN_VERSION;
    static constexpr auto               ms_EDITOR_SCHEMA                = EditorIMPL::ms_EDITOR_SCHEMA;
    //
    //                              MISC. DIMENSIONS:
    static constexpr float              ms_LIST_COLUMN_WIDTH            = 340.0f;   // px width of point‑list column
    static constexpr float              ms_LABEL_WIDTH                  = 90.0f;
    static constexpr float              ms_WIDGET_WIDTH                 = 250.0f;
    //
    //                              SETTINGS TAB:
    static constexpr const char *       ms_NO_ASSIGNED_FILE_STRING      = "UNASSIGNED";      // Used when there is no "File > Save As..." assigned to app...
    static constexpr ImVec2             ms_SETTINGS_BUTTON_SIZE         = ImVec2( 125,   25 );
    //
    //                              OTHER:
    static constexpr ImGuiHoveredFlags  ms_TOOLTIP_HOVER_FLAGS          = ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayNone;
    //
    //
    //  static constexpr const char *       ms_SELECTION_CONTEXT_MENU_ID    = "Editor_Selection_ContextMenu";       //  selection_popup_id
    //  static constexpr const char *       ms_CANVAS_CONTEXT_MENU_ID       = "Editor_Canvas_ContextMenu";          //  canvas_popup_id
    //  static constexpr const char *       ms_BROWSER_CONTEXT_MENU_ID      = "Editor_Browser_ContextMenu";
    //  static constexpr const char *       ms_SYSTEM_PREFERENCES_MENU_ID   = "Editor System Preferences";          //  canvas_popup_id
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    //                              ARRAYS:
    static constexpr auto &             ms_EDITOR_STATE_NAMES               = DEF_EDITOR_STATE_NAMES            ;   //  "Tool" State.
    static constexpr auto &             ms_EDITOR_STATE_HOTKEY_NAMES        = DEF_EDITOR_STATE_HOTKEY_NAMES     ;
    static constexpr auto &             ms_EDITOR_STATE_ICONS               = DEF_EDITOR_STATE_ICONS            ;
    static constexpr auto &             ms_MODE_CAPABILITIES                = DEF_MODE_CAPABILITIES             ;
    static constexpr auto &             ms_TOOLTIP_INFOS                    = DEF_TOOLTIP_INFOS                 ;
    //
    static constexpr auto &             ms_ACTION_STATE_NAMES               = DEF_ACTION_STATE_NAMES            ;   //  Action States.
    static constexpr auto &             ms_OBJECT_TRAIT_NAMES               = DEF_OBJECT_TRAIT_NAMES            ;   //  "Object Trait" Categories.
    //
    //
    //
    //                              VERTEX_T  AND  PATH_T  ARRAYS:
    static constexpr auto &             ms_BEZIER_CURVATURE_TYPE_NAMES      = DEF_BEZIER_CURVATURE_TYPE_NAMES   ;   //  For "Vertex_t"...
    static constexpr auto &             ms_PATH_STATE_NAMES                 = path::DEF_PATH_STATE_NAMES        ;   //  For "Path_t"...
    static constexpr auto &             ms_PATH_PAYLOAD_NAMES               = path::DEF_PATH_PAYLOAD_NAMES      ;
    static constexpr auto &             ms_HIT_TYPE_NAMES                   = DEF_HIT_TYPE_NAMES                ;
    //
    static constexpr auto &             ms_VERTEX_STYLES                    = DEF_VERTEX_STYLES                 ;
    //
    //
    //
    //                              MISC.:
    static constexpr auto &             ms_SHAPE_NAMES                      = DEF_EDITOR_SHAPE_NAMES            ;
    //
    static constexpr auto &             ms_IORESULT_NAMES                   = DEF_IORESULT_NAMES                ;
    static constexpr auto &             ms_POPUP_INFOS                      = DEF_EDITOR_POPUP_INFOS            ;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//      1.          TEMPORARY PUBLIC STUFF...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      1.              INITIALIZATION METHODS...
    // *************************************************************************** //
                                        Editor                          (app::AppState & src);
                                        ~Editor                         (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                              RESIDENT OVERLAY DATA:
    // *************************************************************************** //
    //
    //
    //  cblib::EnumArray< Resident, std::unique_ptr<Overlay> >              m_overlays;
    //
    std::array<ResidentEntry, Resident::COUNT>      m_residents { {
        //
        //  Debugger:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  draw_fn         */  {   },                              // "draw_fn" will be dispatched in ctor.
                /*  locked          */  false,
            //
                /*  placement       */  OverlayPlacement::CanvasTL,
                /*  src_anchor      */  Anchor::NorthWest,
                /*  offscreen       */  OffscreenPolicy::Clamp,
                /*  anchor_px       */  ImVec2{ 50.0f,          35.0f }                 //  nudge below bbox
            },
            {//     STYLE...
                /*  display_title   */  "Debugger Resident",
                /*  alpha           */  0.80f,
                /*  background      */  0x00000000,
                /*  win_rounding    */  8
            }
        },
        //
        //  Selection:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  draw_fn         */  {   },                              // "draw_fn" will be dispatched in ctor.
                /*  locked          */  true,
            //
                /*  placement       */  OverlayPlacement::CanvasPoint,
                /*  src_anchor      */  Anchor::North,
                /*  offscreen       */  OffscreenPolicy::Hide,
                /*  anchor_px       */  ImVec2{0,   25}                      //  Window Offset.
            },
            {//     STYLE...
                /*  display_title   */  nullptr,
                /*  alpha           */  0.95f,
                /*  background      */  0x00000000,
                /*  win_rounding    */  10
            }
        },
        //
        //  Shape:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  draw_fn         */  {   },                              // "draw_fn" will be dispatched in ctor.
                /*  locked          */  true,
            //
                /*  placement       */  OverlayPlacement::CanvasBR,
                /*  src_anchor      */  Anchor::SouthEast,
                /*  offscreen       */  OffscreenPolicy::Clamp,
                /*  anchor_px       */  ImVec2{-30.0f,   45.0f}             //  Window Offset.
            },
            {//     STYLE...
                /*  display_title   */  "Shape Tool",
                /*  alpha           */  0.65f,
                /*  background      */  0xFF000000,
                /*  win_rounding    */  5
            }
        },
    //
    //
    //
    //  ** UI-RESIDENTIAL OVERLAY WINDOWS **...
        //  UITraits:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  draw_fn         */  {   },                              // "draw_fn" will be dispatched in ctor.
                /*  locked          */  false,
            //
                /*  placement       */  OverlayPlacement::CanvasTR,
                /*  src_anchor      */  Anchor::NorthEast,
                /*  offscreen       */  OffscreenPolicy::Clamp,
                /*  anchor_px       */  ImVec2{ 8.0f,       35.0f },           //  Offset Position.
            //
            //
                /*  anchor_ws       */  ImVec2{ 0.0f,       0.0f }          //  ws anchor filled each frame
            },
            {//     STYLE...
                /*  display_title   */  "Traits",
                /*  alpha           */  0.80f,
                /*  background      */  0x00000000,
                /*  win_rounding    */  8,
            //
                /*  window_size     */  {       {     { 375.0f,      500.0f },      { { 100.0f,        100.0f },     { 450.0f,       750.0f } }   }      }
            }
        },
        //
        //  UIObjects:
        {   0,                                  //  ID.
            nullptr,                            //  Reference.
            {   /*  draw_fn         */  {   },                              // "draw_fn" will be dispatched in ctor.
                /*  locked          */  false,
            //
                /*  placement       */  OverlayPlacement::CanvasBL,
                /*  src_anchor      */  Anchor::SouthWest,
                /*  offscreen       */  OffscreenPolicy::Clamp,
                /*  anchor_px       */  ImVec2{ 50.0f,      12.5f },                //  Offset Position.
            },
            {//     STYLE...
                /*  display_title   */  "Objects",
                /*  alpha           */  0.80f,
                /*  background      */  0x00000000,
                /*  win_rounding    */  8,
            //
                /*  window_size     */  {       {     { 225.0f,      300.0f },      { { 100.0f,        100.0f },     { 300.0f,       400.0f } }   }      }
            }
        }
    //
    //
    //
    } };
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END TEMPORARY STUFF.



// *************************************************************************** //
//
//
//      1.          CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      IMPORTANT DATA...
    // *************************************************************************** //
    app::AppState &                     CBAPP_STATE_NAME;
    //
    std::vector<Vertex>                 m_vertices;
    std::vector<Point>                  m_points;
    std::vector<Path>                   m_paths;                //  New path container
    std::unordered_set<HandleID>        m_show_handles;         //  List of which glyphs we WANT to display Bezier points for.
    //
    //
    OverlayManager                      m_ov_manager;      //  formerly: "m_overlays".
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SERIALIZATION STUFF...
    // *************************************************************************** //
    cb::FileDialog::Initializer         m_SAVE_DIALOG_DATA              = {
        /* type               = */  cb::FileDialogType::Save,
        /* window_name        = */  "Save Editor Session",
        /* default_filename   = */  "canvas_settings.json",
        /* required_extension = */  "json",
        /* valid_extensions   = */  {".json", ".txt"},
        /* starting_dir       = */  std::filesystem::current_path()
    };
    cb::FileDialog::Initializer         m_OPEN_DIALOG_DATA              = {
        /* type               = */  cb::FileDialogType::Open,
        /* window_name        = */  "Open Editor Session",
        /* default_filename   = */  "",
        /* required_extension = */  "",
        /* valid_extensions   = */  {".json", ".cbjson", ".txt"},
        /* starting_dir       = */  std::filesystem::current_path()
    };
    cb::FileDialog                      m_save_dialog;
    cb::FileDialog                      m_open_dialog;
    //  std::filesystem::path               m_filepath                      = {"../../assets/.cbapp/presets/editor/editor-fdtd_v0.json"};
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF...
    // *************************************************************************** //
    std::string                         WinInfo_uuid                    = "Editor Browser";
    ImGuiWindowFlags                    WinInfo_flags                   = ImGuiWindowFlags_None | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;
    bool                                WinInfo_open                    = true;
    ImGuiWindowClass                    m_window_class;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      APPLICATION STATE...
    // *************************************************************************** //
    //                              OVERALL STATE:
    //  EditorState                     m_S                             = {   };
    Mode                                m_mode                          = Mode::Default;                //  Current Tool.
    Action                              m_action                        = Action::None;                 //  Current Action.
    ObjectTrait                         m_trait_browser                 = ObjectTrait::Properties;      //  Which "Object Category" is open in BIG-BROWSER.
    ObjectTrait                         m_trait_overlay                 = ObjectTrait::Properties;      //  Which "Object Category" is open in OVERLAY-BROWSER.
    //
    //
    //                              MUTABLE / TRANSIENT STATE:
    std::unique_ptr<Interaction>        m_it;
    //
    bool                                m_dragging                      = false;
    bool                                m_lasso_active                  = false;
    bool                                m_pending_clear                 = false;    //  pending click selection state ---
    //
    //                              PEN-TOOL STATE:
    bool                                m_drawing                       = false;
    bool                                m_dragging_handle               = false;
    bool                                m_dragging_out                  = true;
    VertexID                            m_drag_vid                      = 0;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATE OBJECTS...
    // *************************************************************************** //
    //                              SUBSIDIARY STATES:
    std::unique_ptr<MenuState>          m_menu_state                    {   };          //  UNDO/REDO...
    EditorState                         m_editor_S                      {   };          //  <======|    NEW CONVENTION.  Let's use "m_name_S" to denote a STATE variable...
    mutable RenderCTX                   m_render_ctx;
    BrowserState                        m_browser_S                     {   };
    //
    Selection                           m_sel                   ;
    mutable BoxDrag                     m_boxdrag               ;
    MoveDrag                            m_movedrag              ;
    Clipboard                           m_clipboard             ;
    mutable TooltipState<TooltipKey>    m_tooltip                       { DEF_TOOLTIP_INFOS };
    //
    //                              TOOL STATES:
    PenState                            m_pen                   ;
    ShapeState                          m_shape                 ;
    DebuggerState                       m_debugger              ;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STYLE OBJECTS...
    // *************************************************************************** //
    EditorStyle                         m_style                         {   };
    VertexStyle                         m_vertex_style;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      IMPLOT / CANVAS SYSTEM...
    // *************************************************************************** //
    GridState                           m_grid                          = {   };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      VARIABLES FOR SPECIFIC MECHANICS...
    // *************************************************************************** //
    //                              RESIDENT STUFF:
    //  bool                                m_show_sel_overlay              = false;
    //
    //                              LASSO TOOL / SELECTION:
    ImVec2                              m_lasso_start                   = ImVec2(0.0f,  0.0f);
    ImVec2                              m_lasso_end                     = ImVec2(0.0f,  0.0f);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      NEED TO RE-HOUSE !!!
    // *************************************************************************** //
    //                              INDICES:
    std::optional<Hit>                  m_pending_hit;   // candidate under mouse when button pressed   | //  pending click selection state ---
    VertexID                            m_next_id                       = 1;
    PathID                              m_next_pid                      = 1;                    // counter for new path IDs
    //
    //                              BBOX SCALING:
    mutable std::optional<BoxDrag::Anchor>  m_hover_handle              = std::nullopt;
    mutable ImVec2                      m_origin_scr                    = {0.0f,    0.0f};      // screen-space canvas origin                       //  -1 = none, 0-7 otherwise (corners+edges)
    //
    //                              UTILITY:
    //  float                               m_bar_h                         = 0.0f;
    //  ImVec2                              m_avail                         = ImVec2(0.0f, 0.0f);
    //  ImVec2                              m_p0                            = ImVec2(0.0f, 0.0f);
    //  ImVec2                              m_p1                            = ImVec2(0.0f, 0.0f);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      BOOKMARK !!!
    // *************************************************************************** //
        //
        //                              MAIN APPLICATION STATE:
        //  bool                                m_show_grid                     = true;
        //  Interaction                         m_it                            {   };
        //
        //
        //
        //                              PEN-TOOL STATE:
        //  bool                                m_drawing                       = false;
        //  bool                                m_dragging_handle               = false;
        //  bool                                m_dragging_out                  = true;
        //  VertexID                            m_drag_vid                      = 0;
        //
        //
        //
        //                              SELECTION STATE / LASSO TOOL:
        //  bool                                m_dragging                      = false;
        //  bool                                m_lasso_active                  = false;
        //  bool                                m_pending_clear                 = false;    //  pending click selection state ---
        //
        //  std::optional<Hit>                  m_pending_hit;   // candidate under mouse when button pressed   | //  pending click selection state ---
        //  ImVec2                              m_lasso_start                   = ImVec2(0.f, 0.f);
        //  ImVec2                              m_lasso_end                     = ImVec2(0.f, 0.f);
        
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".






// *************************************************************************** //
//
//
//      2.          PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      MAIN API.                       |   "common.cpp" ...
    // *************************************************************************** //
    void                                save                                (void);
    void                                save_as                             (void);
    void                                open                                (void);
    void                                undo                                (void);
    void                                redo                                (void);
    //
    [[nodiscard]] MenuState &           GetMenuState                         (void) const noexcept;
    [[nodiscard]] MenuState &           GetMenuState                         (void) noexcept;
    //
    //
    //
    void                                Begin                               (const char * id = "##EditorCanvas");
    void                                DrawBrowser                         (void);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      "Wrapped" API Functions.        |   "common.cpp" ...
    // *************************************************************************** //
    void                                _file_dialog_handler                (void);
    void                                _save_IMPL                          (void);
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PUBLIC MEMBER FUNCTIONS.


    
// *************************************************************************** //
//
//
//      2.A.            PROTECTED OPERATION MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:

    // *************************************************************************** //
    //      CORE MECHANICS                  |   "editor.cpp" ...
    // *************************************************************************** //
    //
    //                              SMALLER HELPERS / UTILITIES:
    inline void                         _MECH_show_tooltip_info             (void) const noexcept;
    inline void                         _MECH_change_state                  ([[maybe_unused]] const Interaction & );            //  formerly "_mode_switch_hotkeys"
    inline void                         _MECH_dispatch_tool_handler         ([[maybe_unused]] const Interaction & );            //  formerly "_dispatch_mode_handler"
    //
    inline void                         _per_frame_cache_begin              (void) noexcept;
    inline void                         _per_frame_cache_end                (void) noexcept;
    inline void                             _update_action                  ([[maybe_unused]] const Interaction & ) noexcept;
    //
    //
    //                              PRIMARY MECHANIC HANDLERS:
    void                                _MECH_hit_detection                 (const Interaction & ) const;                       //  formerly "update_cursor_select"
    inline void                         _MECH_update_canvas                 ([[maybe_unused]] const Interaction & );            //  * NEW  _MECH  FUNCTION *
    //
    inline void                         _MECH_draw_ui                       ([[maybe_unused]] const Interaction & );            //  formerly "_handle_overlays()"
    inline void                         _MECH_dispatch_action               ([[maybe_unused]] const Interaction & ) noexcept;   //  * NEW  _MECH  FUNCTION *
    inline void                         _MECH_drive_io                      (void);                                             //  formerly "_handle_io()"
    //
    //
    //                              LOCATED ELSEWHERE:
    void                                _MECH_render_frame                  ([[maybe_unused]] const Interaction & ) const;  //  * NEW  _MECH  FUNCTION *    location: "render.cpp".
    void                                _MECH_pump_main_tasks               (void);                     //  formerly "pump_main_tasks".                     location: "serialization.cpp".
    void                                _MECH_draw_controls                 (void);                     //  formerly "_draw_controls".                      location: "browser.cpp".
    void                                _MECH_process_selection             (const Interaction & );     //  formerly "_process_selection".                  location: "selection.cpp".
    void                                _MECH_query_shortcuts               ([[maybe_unused]] const Interaction & );        //  * NEW  _MECH  FUNCTION *    location: "shortcuts.cpp".
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      TOOL / "MODE" HANDLERS.         |   "editor.cpp" ...
    // *************************************************************************** //
    inline void                         _handle_default                     (const Interaction & );
    //
    inline void                         _handle_hand                        ([[maybe_unused]] const Interaction & );
    inline void                         _handle_pen                         (const Interaction & );
    inline void                         _handle_scissor                     (const Interaction & );
    inline void                         _handle_add_anchor                  ([[maybe_unused]] const Interaction & );
    inline void                         _handle_remove_anchor               ([[maybe_unused]] const Interaction & );
    inline void                         _handle_edit_anchor                 ([[maybe_unused]] const Interaction & );
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      BROWSER STUFF.                  |   "browser.cpp" ...
    // *************************************************************************** //
    //                              BROWSER ORCHESTRATORS:
    void                                _dispatch_obj_inspector_column          (void);
    void                                _dispatch_obj_inspector_column          (ObjectTrait & );     //  PREVIOUSLY:     _draw_path_inspector_column
    //
    //                              OBJECT SELECTION:
    void                                _draw_obj_selector_table                (void);
    inline void                             _draw_obj_selectable                    (Path & , const int , const bool , const bool);
    //
    //                              TRAIT BEHAVIORS:
    inline void                         _draw_trait_selector                    (ObjectTrait & );
    void                                _dispatch_trait_inspector               (ObjectTrait &, const LabelFn & );
        inline void                         _dispatch_trait_inspector_single        (ObjectTrait &, const LabelFn & );
        inline void                         _dispatch_trait_inspector_multi         (ObjectTrait &, const LabelFn & );
    //
    //
    //                              UTILITY:
    inline void                         _draw_control_tooltips                  (const TooltipKey , const bool state) const noexcept;
    //
    //
    //
    //                              TEMPORARY:
    void                                _show_browser_color_edit_window         (void);     //  TEMPORARY...
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      BROWSER-PANEL STUFF.            |   "browser_panel.cpp" ...
    // *************************************************************************** //
    //                              "PROPERTIES" TRAIT:
    void                                _draw_properties_panel_single           (Path & , const size_t , const LabelFn & );
    void                                _draw_properties_panel_multi            ([[maybe_unused]] const LabelFn & );      //  PREVIOUSLY:     _draw_multi_path_inspector
    //
    //                              "VERTICES" TRAIT:
    void                                _draw_vertex_panel                      (Path & path, [[maybe_unused]] const size_t , const LabelFn & );
    void                                _draw_vertex_selector_column            (Path & , const size_t );  //  PREVIOUSLY:     _draw_vertex_list_subcolumn
    void                                _draw_vertex_inspector_column           (Path & , const LabelFn & );  //  PREVIOUSLY:     _draw_vertex_inspector_subcolumn
    inline void                             _draw_vertex_properties_panel       (Vertex & , const LabelFn & );
    //
    //                              "PAYLOAD" TRAIT:
    void                                _draw_payload_panel                     (Path & path, [[maybe_unused]] const size_t , const LabelFn & );
    //
    //
    //                              BROWSER HELPERS:
    void                                _handle_selection_click                 (int row_idx, bool mutable_path);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      PEN-TOOL STUFF.                 |   "pen_tool.cpp" ...
    // *************************************************************************** //
    void                                _pen_begin_handle_drag              (VertexID vid, bool out_handle, const bool force_select=false);
    //bool                                _pen_try_begin_handle_drag          (const Interaction & );
    void                                _pen_update_handle_drag             ([[maybe_unused]] const Interaction & );
    void                                _pen_begin_path_if_click_empty      (const Interaction & );
    void                                _pen_append_or_close_live_path      (const Interaction & );
    //
    std::optional<size_t>               _path_idx_if_last_vertex            (VertexID vid) const;
    inline bool                         _pen_click_hits_first_vertex        (const Interaction &, const Path &) const;
    inline bool                         _can_join_selected_path             (void) const;
    void                                _join_selected_open_path            (void);
    void                                _draw_pen_cursor                    (const ImVec2 &, ImU32);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SHAPE TOOL STUFF.               |   "tools.cpp" ...
    // *************************************************************************** //
    //                              MAIN SHAPE-TOOL FUNCTIONS:
    void                                _handle_shape                       ([[maybe_unused]] const Interaction & );
    void                                _shape_begin_anchor                 ([[maybe_unused]] const Interaction & );
    void                                _shape_update_radius                ([[maybe_unused]] const Interaction & );
    //
    void                                _shape_commit                       (void);
    void                                _shape_reset                        (void);
    //
    //                              SPECIFIC SHAPE FUNCTIONS:
    VertexID                            _shape_add_vertex                   (const ImVec2 & ws);
    size_t                              _shape_build_rectangle              (const ImVec2 & cen, float r);
    size_t                              _shape_build_ellipse                (const ImVec2 & cen, float r);
    //
    //                              UTILITIES:
    //                                  ...
    //
    //                              DEPRECATED:
    void                                _shape_preview_draw                 (ImDrawList * dl) const;
    void                                _draw_shape_cursor                  (const Interaction &) const;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MENUS & CONTEXT-WINDOW STUFF.   |   "menus.cpp" ...
    // *************************************************************************** //
    //                              CONTEXT MENU ORCHESTRATORS:
    void                                dispatch_selection_context_menus    ([[maybe_unused]] const Interaction & it);
    //
    //                              CANVAS CONTEXT MENUS:
    inline void                         _show_canvas_context_menu           ([[maybe_unused]] const Interaction & it, const char * );
    //
    //                              SELECTION CONTEXT MENUS:
    inline void                         _show_selection_context_menu        ([[maybe_unused]] const Interaction & it, const char * );
    inline void                             _selection_context_primative        ([[maybe_unused]] const Interaction & );
    //
    //                                  Single---Selection.
    inline void                             _selection_context_single           ([[maybe_unused]] const Interaction & ) noexcept;
    inline void                             _selection_context_single_advanced  ([[maybe_unused]] const Interaction & , Path & ) noexcept;
    //
    //                                  Multi---Selection.
    inline void                             _selection_context_multi            ([[maybe_unused]] const Interaction & );
    //
    //                              MISC. CONTEXT MENUS:
    bool                                _show_tool_selection_menu           (const Mode) noexcept;
    //
    //
    //
    //                              CUSTOM MENUBAR MENUS:
    void                                _MENUBAR_object_menu                (void) noexcept;
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RESIDENT STUFF.                 |   "resident.cpp" ...
    // *************************************************************************** //
    //                              UTILITY:
    void                                _dispatch_resident_draw_fn          (Resident idx);
    //
    //                              DEBUGGER RESIDENT:
    void                                _draw_debugger_resident             (void);
    void                                _debugger_draw_controlbar           (void);
    void                                    _DEBUGGER_state                 (void) const noexcept;
    inline void                                 _DEBUGGER_state_dragging    (const float , const float ) const noexcept;
    void                                    _DEBUGGER_canvas                (void) const noexcept;
    void                                    _DEBUGGER_misc                  (void) const noexcept;
    inline void                                 _DEBUGGER_misc_1            (const float , const float ) const noexcept;
    inline void                                 _DEBUGGER_misc_2            (const float , const float ) const noexcept;
    inline void                                 _DEBUGGER_misc_3            (const float , const float ) const noexcept;
    //
    void                                    _debugger_hit_detection             (void);
    void                                    _debugger_interaction               (void);
    void                                    _debugger_more_info                 (void);
    //
    //                              DEBUGGER UTILITIES:
    inline bool                             GetOpenMenuNames                (std::string & ) const noexcept;
    //
    //
    //                              SELECTION RESIDENT:
    void                                _draw_selection_resident            (void);
    //
    //                              SHAPE-TOOL RESIDENT:
    void                                _draw_shape_resident                (void);
    void                                    _draw_shape_resident_custom         (void);
    void                                    _draw_shape_resident_multi          (void);
    void                                    _draw_shape_resident_default        (void);
    //
    //                              UI-TOOLS RESIDENTS:
    void                                    _draw_ui_traits_resident        (void);
    void                                    _draw_ui_objects_resident       (void);
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      RENDERING FUNCTIONS.            |   "render.cpp" ...
    // *************************************************************************** //
    struct RenderCache
    {
        std::vector<size_t>     z_view;                 // indices into m_paths
        size_t                  n_paths_last    = 0;
        bool                    dirty           = true;
    };
    mutable RenderCache                 m_render_cache                      {   };      //  mutable if _MECH_render_frame() is const
    //
    //
    //
    //                              NEW RENDERING FUNCTIONS:
    inline void                         _RENDER_update_render_cache         (void) const noexcept;
    //
    inline void                         _RENDER_grid_channel                (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                         _RENDER_object_channel              (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                         _RENDER_highlights_channel          (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                         _RENDER_features_channel            (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                         _RENDER_accents_channel             (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                             _ACCENTS_default                (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                             _ACCENTS_geometry               (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                             _ACCENTS_all_objects            (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                             _ACCENTS_all_handles            (std::span<const size_t> , const RenderCTX & ) const noexcept;
    //
    inline void                         _RENDER_glyphs_channel              (std::span<const size_t> , const RenderCTX & ) const noexcept;
    inline void                         _RENDER_top_channel                 (std::span<const size_t> , const RenderCTX & ) const noexcept;
    //
    //
    //                              PRIMARY RENDERING:
#ifndef _EDITOR_REMOVE_DEPRECATED_CODE
    void                                _render_paths                       (ImDrawList * dl) const;
    void                                _render_lines                       (ImDrawList *, const ImVec2 & ) const;
    void                                _render_points                      (ImDrawList *) const;
#endif  //  _EDITOR_REMOVE_DEPRECATED_CODE  //
    //
    //
    //                              SELECTION RENDERING:
    void                                render_selection_highlight          (ImDrawList * , const RenderCTX & ) const noexcept;
    inline void                             _render_selection_objects       (const RenderCTX & ) const noexcept;  //  Helper for "render_selection_highlight"
    inline void                             _render_selected_handles        (ImDrawList * ) const noexcept;  //  Helper for "render_selection_highlight"
    inline void                             _render_selection_bbox          (ImDrawList * ) const noexcept;  //  Helper for "render_selection_highlight"
    //
    inline void                         _render_auxiliary_highlights        (const RenderCTX & ) const noexcept;   //  Helper for "render_selection_highlight"
    inline void                             _auxiliary_highlight_object     (const Path & ,   const RenderCTX & ) const noexcept;
    inline void                             _auxiliary_highlight_handle     (const Vertex & ) const noexcept;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      HIT-DETECTION MECHANICS.        |   "hit_detection.cpp" ...
    // *************************************************************************** //
    //                              PRIMARY HIT-DETECTION MECHANISMS:
    [[nodiscard]] inline bool           _hit_bbox_handle                    ([[maybe_unused]] const Interaction & ) const;
    int                                 _hit_point                          ([[maybe_unused]] const Interaction & ) const;
    std::optional<Hit>                  _hit_any                            ([[maybe_unused]] const Interaction & ) const;
    std::optional<PathHit>              _hit_path_segment                   ([[maybe_unused]] const Interaction & ) const;
    //
    //                              HIT-DETECTION UTILITIES:
    
    inline void                         _dispatch_cursor_hint               (const Hit::Type) const noexcept; 
    inline void                         _dispatch_cursor_icon               ([[maybe_unused]] const Interaction & ) const;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SELECTION MECHANICS.            |   "selection.cpp" ...
    // *************************************************************************** //
    //                              MAIN SELECTION OPERATIONS:
    inline void                         resolve_pending_selection           (const Interaction & it);
    inline void                         update_move_drag_state              (const Interaction & it);
    void                                start_move_drag                     (const ImVec2 & anchor_ws);
    void                                add_hit_to_selection                (const Hit & hit);
    void                                _rebuild_vertex_selection           (void);   // decl
    //
    //                              SELECTION HIGHLIGHT / USER-INTERACTION / APPEARANCE:
    bool                                _selection_bounds                   (ImVec2 & tl, ImVec2 & br, const RenderCTX & ) const;
    //
    //                              BOUNDING BOX MECHANICS:
    void                                _start_bbox_drag                    (const BoxDrag::Anchor , const ImVec2 , const ImVec2 );
    void                                _update_bbox                        (void);
    //
    //                              LASSO TOOL MECHANICS:
    void                                _start_lasso_tool                   (void);
    void                                _update_lasso                       (const Interaction & );
    //
    //                              NEW HELPER FUNCTIONS:
    inline float                        _drag_threshold_px                  (void) const;
    inline bool                         _press_inside_selection             (const ImVec2 & ) const;
    inline bool                         _hit_is_in_current_selection        (const Hit & ) const;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SHORTCUT MECHANICS.            |   "shortcuts.cpp" ...
    // *************************************************************************** //
    //                              SUBSIDIARY SHORTCUT HANDLERRS:
    inline void                         _selection_no_selection_shortcuts   ([[maybe_unused]] const Interaction & );
    inline void                             _selection_grid_shortcuts           ([[maybe_unused]] const Interaction & ) noexcept;   //  PREVIOUSLY: "_grid_handle_shortcuts"
    //
    inline void                         _selection_read_only_shortcuts      ([[maybe_unused]] const Interaction & );
    inline void                         _selection_mutable_shortcuts        ([[maybe_unused]] const Interaction & );
    inline void                         _selection_advanced_shortcuts       ([[maybe_unused]] const Interaction & );
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      SERIALIZATION STUFF.            |   "serialization.cpp" ...
    // *************************************************************************** //
    struct SettingsData {
        EditorState &       ES          ;
        GridState &         GS          ;
        EditorStyle &       Style       ;
        float &             LABEL_W     ;
        float &             WIDGET_W    ;
    };
    //
    //
    //                              EDITOR SETTINGS:
    void                                _draw_editor_settings               ([[maybe_unused]] popup::Context & ctx);
    //
    //                              HEADER 1, "PROJECT DATA":
    inline void                         _settings_H1                        (SettingsData & );
    inline void                             _H1_project_data                (SettingsData & );
    //
    //                              HEADER 2, "EDITOR SETTINGS":
    inline void                         _settings_H2                        (SettingsData & );
    inline void                             _H2_state                       (SettingsData & );
    inline void                             _H2_mechanics                   (SettingsData & );
    //
    //                              HEADER 3, "USER PREFERENCES":
    inline void                         _settings_H3                        (SettingsData & );
    //
    //                              HEADER 4, "OPERATIONS":
    inline void                         _settings_H4                        (SettingsData & );
    //
    //
    //
    //                              SERIALIZATION ORCHESTRATORS:
    void                                _draw_io_overlay                    (void);
    //
    inline EditorSnapshot               make_snapshot                       (void) const;
    void                                load_from_snapshot                  (EditorSnapshot && );
    //
    //                              SERIALIZATION IMPLEMENTATIONS:
    bool                                save_async                          (std::filesystem::path path);
    void                                save_worker                         (EditorSnapshot snap, std::filesystem::path path);
    //
    bool                                load_async                          (std::filesystem::path path);
    void                                load_worker                         (std::filesystem::path );
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      UTILITIES.                      |   "utility.cpp" ...
    // *************************************************************************** //
    //                              GENERAL APPLICATION HELPERS:
    //  Vertex *                            find_vertex                         (std::vector<Vertex> & , VertexID);
    //  const Vertex *                      find_vertex                         (const std::vector<Vertex> & , VertexID) const;
    std::optional<EndpointInfo>         _endpoint_if_open                   (PathID vid) const;
    //
    //                              DATA MODIFIER UTILITIES
    void                                _add_point_glyph                    (VertexID vid);
    VertexID                            _add_vertex                         (ImVec2);
    Vertex &                            _add_vertex_return_reference        (ImVec2) noexcept;
    const Vertex &                      _add_vertex_return_const_reference  (ImVec2) noexcept;
    //
    void                                _add_point                          (ImVec2 w);
    void                                _erase_vertex_and_fix_paths         (VertexID vid);
    void                                _erase_path_and_orphans             (VertexID vid);
    //
    //                              MISC. UTILITIES:
    bool                                _try_begin_handle_drag              (const Interaction & );
    void                                _scissor_cut                        (const PathHit & );
    void                                _update_world_extent                (void);
    //
    //                              LOCAMOTION UTILITIES:
    void                                _utl_set_canvas_window              (void) noexcept;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      COMMON.                         |   "common.cpp" ...
    // *************************************************************************** //
    //                              NEW SELECTION FUNCTIONS:
    void                                bring_selection_to_front            (void);
    void                                bring_selection_forward             (void);
    void                                send_selection_backward             (void);
    void                                send_selection_to_back              (void);
    //
    //                              SELECTION FUNCTIONS:
    void                                move_selection                      (const float dx, const float dy);
    void                                copy_to_clipboard                   (void);
    void                                paste_from_clipboard                (ImVec2);
    void                                delete_selection                    (void);
    //
    //                              GLOBAL OPERATIONS:
    void                                _clear_all                          (void);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END PROTECTED MEMBER FUNCTIONS.


   
// *************************************************************************** //
//
//
//      2.C.            INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      NEW BBOX STUFF...
    // *************************************************************************** //
    
    //  Add tiny revision counters in Editor (to drive “dirty”)
    //      We’ll bump these when selection/geometry/camera/style change.
    //      For now, just add them and bump the camera rev every frame; we’ll wire the others in a later tiny step.
    //
    uint64_t                            m_rev_sel                               = 1;   // selection changes
    uint64_t                            m_rev_geom                              = 1;   // vertex/path geometry changes
    uint64_t                            m_rev_cam                               = 1;   // plot limits / zoom / pan
    uint64_t                            m_rev_style                             = 1;   // handle sizes, bbox margin, colors

    //  Small helpers (use as we wire things up)
    inline void                         _rev_bump_sel                           (void) noexcept     { ++m_rev_sel;   }
    inline void                         _rev_bump_geom                          (void) noexcept     { ++m_rev_geom;  }
    inline void                         _rev_bump_cam                           (void) noexcept     { ++m_rev_cam;   }
    inline void                         _rev_bump_style                         (void) noexcept     { ++m_rev_style; }
    
    
    
    //  "_selbox_rebuild_view_if_needed"
    void                                _selbox_rebuild_view_if_needed          (const Interaction & it);
    
    //  "_expand_bbox_by_pixels"
    inline std::pair<ImVec2, ImVec2>    _expand_bbox_by_pixels                  (const ImVec2& tl_ws_in, const ImVec2& br_ws_in, float margin_px) const {
        ImVec2 p0 = world_to_pixels(tl_ws_in);
        ImVec2 p1 = world_to_pixels(br_ws_in);
        ImVec2 min_px{ std::min(p0.x, p1.x), std::min(p0.y, p1.y) };
        ImVec2 max_px{ std::max(p0.x, p1.x), std::max(p0.y, p1.y) };
        min_px.x -= margin_px;  min_px.y -= margin_px;
        max_px.x += margin_px;  max_px.y += margin_px;
        return { pixels_to_world(min_px), pixels_to_world(max_px) };
    }
    
    // *************************************************************************** //



    // *************************************************************************** //
    //      NEW INLINE FUNCS...
    // *************************************************************************** //
    
    /*{
    inline std::pair<ImVec2, ImVec2>    _expand_bbox_by_pixels                  (const ImVec2& tl_ws_in, const ImVec2& br_ws_in, float margin_px) const
        ImVec2 p0 = world_to_pixels(tl_ws_in);
        ImVec2 p1 = world_to_pixels(br_ws_in);

        // Normalize to pixel-space min/max first (left/top, right/bottom)
        ImVec2 min_px{ std::min(p0.x, p1.x), std::min(p0.y, p1.y) };
        ImVec2 max_px{ std::max(p0.x, p1.x), std::max(p0.y, p1.y) };

        // Expand outward in pixel space
        min_px.x -= margin_px;  min_px.y -= margin_px;
        max_px.x += margin_px;  max_px.y += margin_px;

        // Convert back; these are world TL (left, top) and BR (right, bottom)
        ImVec2 tl_ws_out = pixels_to_world(min_px);
        ImVec2 br_ws_out = pixels_to_world(max_px);
        return { tl_ws_out, br_ws_out };
    }*/
    

    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      QUERY FUNCTIONS...
    // *************************************************************************** //
    
    //  "IsDragging"
    //
    [[nodiscard]] inline bool           IsDragging                              (void) const noexcept {
        return ( this->IsDraggingSelection() || this->IsScalingSelection() || this->IsDraggingVertex() ||
                 this->IsDraggingHandle()    || this->IsDraggingLasso() );
    }
    //
    //  "IsDraggingSelection"
    [[nodiscard]] inline bool           IsDraggingSelection                     (void) const noexcept   { return ( this->m_dragging                 );          }
    [[nodiscard]] inline bool           IsScalingSelection                      (void) const noexcept   { return ( this->m_boxdrag.IsScaling()      );          }
    //
    //  [[nodiscard]] inline bool           IsDraggingVertex                        (void) const noexcept   { return static_cast<bool>( this->m_drag_vid > 0 );     }
    [[nodiscard]] inline bool           IsDraggingVertex                        (void) const noexcept   { return ( this->m_dragging                 );          }
    [[nodiscard]] inline bool           IsDraggingHandle                        (void) const noexcept   { return ( this->m_dragging_handle          );          }
    [[nodiscard]] inline bool           IsDraggingLasso                         (void) const noexcept   { return ( this->m_lasso_active             );          }
    
    
    
    //  "IsDrawing"
    //
    [[nodiscard]] inline bool           IsDrawing                               (void) const noexcept {
        return ( this->IsDrawingPen() || this->IsDrawingShape() );
    }
    //
    //  "IsDrawingPen"
    [[nodiscard]] inline bool           IsDrawingPen                            (void) const noexcept   { return ( /*this->m_mode == Mode::Pen  &&    */ this->m_pen.active     );  }
    [[nodiscard]] inline bool           IsDrawingShape                          (void) const noexcept   { return ( /*this->m_mode == Mode::Shape  &&  */ this->m_shape.active   );  }
    
    
    
    //  "NoMenusOpen"
    [[nodiscard]] inline bool           NoMenusOpen                             (void) const noexcept   { return (*this->m_it).NoMenusOpen(); }
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      INLINE GRID/WORLD FUNCTIONS...
    // *************************************************************************** //
    
    //  "world_to_pixels"
    //      ImPlot works in double precision; promote, convert back to float ImVec2
    static inline ImVec2                world_to_pixels                         (ImVec2 w) noexcept {
        ImPlotPoint p = ImPlot::PlotToPixels(ImPlotPoint(w.x, w.y));
        return { static_cast<float>(p.x), static_cast<float>(p.y) };
    }
    
    //  "find_vertex"
    static inline Vertex *              find_vertex                             (std::vector<Vertex> & verts, VertexID id) noexcept
        { for (auto & v : verts) {  if (v.id == id) {return &v;}  } return nullptr; }
    //
    static inline const Vertex *        find_vertex                             (const std::vector<Vertex> & verts, VertexID id) noexcept
        { for (auto & v : verts) if (v.id == id) return &v; return nullptr; }


    
    
    
    
    //  "pixels_to_world"
    inline ImVec2                       pixels_to_world                         (ImVec2 scr) const {
        ImPlotPoint p = ImPlot::PixelsToPlot(scr);// ImPlot uses double; convert back to float for our structs
        return { static_cast<float>(p.x), static_cast<float>(p.y) };
    }
    
    //  "snap_to_grid"
    [[nodiscard]] inline ImVec2         snap_to_grid                            (ImVec2 ws) const
    {
        const GridState &   GS      = this->m_grid;
        const bool          snap    = this->want_snap();
        
        if ( snap ) {
            ws.x    = cblib::math::quantize( ws.x,  GS.m_grid_spacing[0] );
            ws.y    = cblib::math::quantize( ws.y,  GS.m_grid_spacing[1] );
        }
        else if ( GS.pixel_perfect ) {
            ws.x    = cblib::math::quantize( ws.x,  1.0f );
            ws.y    = cblib::math::quantize( ws.y,  1.0f );
        }
        
        return ws;
    }
    /*{
        if ( this->want_snap() ) {
            float s = m_grid.snap_step;
            if ( s <= 0.0f )    { return ws; }                    // safety
            const float inv = 1.0f / s;
            ws.x = std::round(ws.x * inv) * s;
            ws.y = std::round(ws.y * inv) * s;
        }
        return ws;
    }*/
    
    
    //  "want_snap"
    inline bool                         want_snap                               (void) const noexcept
        { return m_grid.snap_on || ImGui::GetIO().KeyShift; }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      INLINE BROWSER FUNCTIONS...
    // *************************************************************************** //
    
    //  "_ensure_paths_sorted_by_z_desc"
    //
    inline bool _ensure_paths_sorted_by_z_desc(void)
    {
        const int               n               = static_cast<int>( m_paths.size() );
        
        if ( n <= 1 )           { return false; }




        //  Build current order and check if already sorted (desc by z_index)
        std::vector<int>        order           (n);
        for (int i = 0; i < n; ++i)             { order[i] = i; }

        auto is_desc_sorted = [&]() -> bool {
            for (int i = 1; i < n; ++i) {
                if (m_paths[i-1].z_index < m_paths[i].z_index)   // should be non-increasing
                    return false;
            }
            return true;
        };
        if ( is_desc_sorted() )                 { return false; }


        //  Sort by z_index desc, stable for equal z
        std::stable_sort(order.begin(), order.end(),
            [&](int a, int b) { return m_paths[a].z_index > m_paths[b].z_index; });


        //  Build new container and a map old_idx -> new_idx
        std::vector<Path>       new_paths;
        new_paths.reserve(n);
        std::vector<int>        new_of_old      (n, -1);
        
        
        for ( int new_i = 0; new_i < n; ++new_i ) {
            int old_i = order[new_i];
            new_of_old[old_i] = new_i;
            new_paths.push_back(std::move(m_paths[old_i]));
        }
        m_paths.swap(new_paths);


        //  Reassign sequential z so top row = greatest z
        for (int i = 0; i < n; ++i) {
            m_paths[i].z_index = Z_FLOOR_USER + (n - 1 - i);
        }


        //  Remap selection indices
        std::unordered_set<size_t>      remapped;
        remapped.reserve(m_sel.paths.size());
        for (size_t old_idx : m_sel.paths) {
            int new_idx = new_of_old[static_cast<int>(old_idx)];
            if (new_idx >= 0) remapped.insert(static_cast<size_t>(new_idx));
        }
        m_sel.paths.swap(remapped);

        // Remap BrowserState indices that reference OBJECT rows
        BrowserState &  BS          = m_browser_S;
        auto            remap_idx   = [&](int & idx) {
            if (idx < 0)    { return; }
            idx = new_of_old[idx];
            if (idx < 0)    { idx = -1; }
        };
        remap_idx(BS.m_browser_anchor);
        remap_idx(BS.m_obj_rename_idx);
        // (Layer-specific indices are unrelated to the object list, so we leave them.)

        return true;
    }


    //  "_reorder_paths"
    inline void                         _reorder_paths                      (int src, int dst)
    {
        BrowserState &      BS          = m_browser_S;
        const bool          invalid     = ( (src == dst) || (src < 0) || (dst < 0) || ( src >= static_cast<int>(m_paths.size()) ) || ( dst >= static_cast<int>(m_paths.size()) ) );
        
        if ( invalid )      { return; }


        //      1.      REORDER THE VECTOR.
        Path temp      = std::move(m_paths[src]);
        m_paths.erase( m_paths.begin() + src );
        m_paths.insert( m_paths.begin() + (src < dst ? dst - 1 : dst), std::move(temp) );


        //      2.      REBUILD Z-INDEX     [ TOP-ROW = HIGHEST Z-INDEX.  BOTTOM-ROW = LOWEST Z-INDEX ].
        const int n = static_cast<int>(m_paths.size());
        for (int i = 0; i < n; ++i) {
            m_paths[i].z_index = Z_FLOOR_USER + (n - 1 - i);
        }


        //      3.      ADJUST EDITOR-WIDE INDICES.
        auto remap = [&](int & idx)
        {
            if ( idx < 0 )                                      { return;       }
            if ( idx == src )                                   { idx = dst;    }
            else if ( src < dst && idx > src && idx <= dst )    { idx--;        }
            else if ( dst < src && idx >= dst && idx <  src )   { idx++;        }
            return;
        };
        
        
        remap(BS.m_browser_anchor);
        remap(BS.m_layer_browser_anchor);
        remap(BS.m_obj_rename_idx);
        remap(BS.m_layer_rename_idx);

        //      4.      REMAP PATH-SELECTION SET.
        std::unordered_set<size_t> new_sel;
        for (size_t old_idx : m_sel.paths)
        {
            int idx = static_cast<int>(old_idx);
            remap(idx);
            new_sel.insert(static_cast<size_t>(idx));
        }
        m_sel.paths.swap(new_sel);
        
        
        return;
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      NEW BOOK-KEEPING FUNCTIONS...
    // *************************************************************************** //
    
    //  "next_z_index"
    [[nodiscard]] inline ZID            next_z_index                        (void)
    {
        ZID max_z = Z_FLOOR_USER;       // find current maximum among user objects
        for (const Path& p : m_paths)
            if (p.z_index > max_z) max_z = p.z_index;

        if (max_z >= Z_CEIL_USER - 1)   // bump; renormalise if we’re at the ceiling
            renormalise_z();

        return std::min(max_z + 1, Z_CEIL_USER - 1);
    }

    //  "renormalise_z"
    void                                renormalise_z                       (void)
    {
        std::vector<Path*>      items;// collect visible+hidden paths in stable draw order
        items.reserve(m_paths.size());
        for (Path & p : m_paths)    { items.push_back(&p); }

        std::stable_sort(items.begin(), items.end(),
            [](const Path* a, const Path* b){ return a->z_index < b->z_index; });

        ZID     z   = Z_FLOOR_USER;
        for (Path * p : items)      { p->z_index = z++; }
        return;
    }
    
    //  "parent_path_of_vertex"
    [[nodiscard]] inline const Path *   parent_path_of_vertex               (VertexID vid) const noexcept {
        for (const Path & p : m_paths) {
            for (VertexID v : p.verts)      { if (v == vid) return &p; }
        }
        return nullptr;                     // not found
    }
    
    //  "parent_path_of_vertex_mut"
    //      Mutable variant – returns nullptr if not found
    [[nodiscard]] inline Path *         parent_path_of_vertex_mut           (VertexID vid) {
        for (Path & p : m_paths) {
            for (VertexID v : p.verts) {
                if (v == vid)   { return &p; }
            }
        }
        return nullptr;
    }
    
    //  "_erase_vertex_record_only"
    inline void                         _erase_vertex_record_only           (VertexID vid) {
        m_vertices.erase(std::remove_if(m_vertices.begin(), m_vertices.end(), [vid](const Vertex& v){ return v.id == vid; }), m_vertices.end());
    }
    
    //  "_prune_selection_mutability"
    inline void                         _prune_selection_mutability         (void)
    {
        //  1.  PATHS...
        for ( auto it = m_sel.paths.begin(); it != m_sel.paths.end(); )
        {
            PathID  pid     = static_cast<PathID>(*it);
            if ( pid >= m_paths.size() || !m_paths[pid].IsMutable() )       { it = m_sel.paths.erase(it); }
            else                                                            { ++it; }
        }

        //  2.  POINTS & VERTICES...
        for ( auto it = m_sel.points.begin(); it != m_sel.points.end(); )
        {
            PointID         idx     = static_cast<PointID>(*it);
            if ( idx >= m_points.size() )                                   { it = m_sel.points.erase(it); continue; }

            const Path *    pp      = parent_path_of_vertex(m_points[idx].v);
            if ( !pp || !pp->IsMutable() )                                  { it = m_sel.points.erase(it); }
            else                                                            { ++it; }
        }
        for ( auto it = m_sel.vertices.begin(); it != m_sel.vertices.end(); )
        {
            VertexID        vid     = static_cast<VertexID>(*it);
            const Path *    pp      = parent_path_of_vertex(vid);
            if ( !pp || !pp->IsMutable() )                                  { it = m_sel.vertices.erase(it); }
            else                                                            { ++it; }
        }
        
        
        //      4.      IF NO OBJECTS REMAIN IN SELECTION, CLOSE SELECTION WINDOW...
        //  this->m_editor_S.m_show_sel_overlay = static_cast<bool>( this->m_sel.paths.size() == 0 );


        _rebuild_vertex_selection();     // sync vertices ↔ points
        return;
    }
    
    //  "_new_path_from_prototype"
    [[nodiscard]] inline Path           _new_path_from_prototype            (const Path & proto) {
        Path    p   = proto;                    //  Copy style, verts cleared by caller
        p.set_default_label(m_next_pid++);      //  "Path N"
        return p;
    }
    
    //  "_clone_path_proto"
    [[nodiscard]] inline Path           _clone_path_proto                   (const Path& src) {
        Path p = src;                     // copy style, z_index, locked, visible
        p.id = m_next_pid++;              // fresh PathID
        p.set_default_label(p.id);        // "Path %u"
        p.verts.clear();                  // caller will fill verts
        return p;
    }
    
    //  "_recompute_next_ids"
    inline void                         _recompute_next_ids                 (void) {
        m_next_id   = 1;
        m_next_pid  = 1;
        for (const Vertex& v : m_vertices)  { if (v.id >= m_next_id)    { m_next_id  = v.id + 1; }      }
        for (const Path& p : m_paths)       { if (p.id >= m_next_pid)   { m_next_pid = p.id + 1; }      }
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      STANDARDIZED MECHANICS FOR TOOLS...
    // *************************************************************************** //
    
    //  "reset_pen"
    inline void                         reset_pen                           (void) {
        this->m_show_handles    .erase( m_pen.handle_vid );
        this->m_sel.vertices    .erase( m_pen.last_vid );
        this->m_pen             .reset();   //  m_pen = {};
        return;
    }
    
    //  "reset_selection"
    inline void                         reset_selection                     (void) {
        this->m_sel.clear();                            this->m_show_handles.clear();
        this->m_editor_S.m_show_sel_overlay = false;    this->m_browser_S.reset();
        this->m_editor_S.m_show_sel_overlay = false;    this->m_browser_S.reset();
        return;
    }
    
    //  "RESET_ALL"
    inline void                         RESET_ALL                           (void) {
        this->reset_pen();
        this->reset_selection();
        this->m_clipboard.clear();
        this->m_shape = {};
        
        this->_clear_all();
    
        return;
    }
    
    //  "_make_path"
    //      Always call this to materialise a new Path.
    //      • Assigns fresh PathID & label
    //      • Puts it on the top Z-layer (unless caller overrides)
    //      • Copies style / flags from an optional prototype
    //
    inline Path &                       _make_path                          (const std::vector<VertexID> & verts, const Path * proto = nullptr) {
        Path        p       = proto ? *proto : Path{};      // copy style/flags if given
        p.id                = m_next_pid++;
        p.set_default_label(p.id);
        p.closed            = false;                        // caller may flip later
        p.z_index           = next_z_index();

        p.verts             = verts;                        // freshly-built vertex list
        m_paths.push_back(std::move(p));
        return m_paths.back();                              // reference for caller tweaks
    }
    
    //  "_make_shape"
    inline Path &                       _make_shape                         (const std::vector<VertexID> & verts, const Path * proto = nullptr) {
        Path &  p               = _make_path(verts, proto);   // assigns id, label, z_index, flags
        p.closed                = true;
        p.style.fill_color      = Path::ms_DEF_PATH_FILL_COLOR;
        return p;
    }
    
    //  "_clone_path"
    //      Duplicate an existing path with a vid-remap (copy/paste, boolean ops …).
    //
    inline Path &                       _clone_path                         (const Path & src, const std::vector<VertexID> & vid_map) {
        Path dup = src;                // copy style / flags
        dup.id   = m_next_pid++;
        dup.set_default_label(dup.id);
        dup.z_index = next_z_index();

        dup.verts.clear();
        for (VertexID old : src.verts)
            dup.verts.push_back(vid_map[old]); // map to duplicated verts

        m_paths.push_back(std::move(dup));
        return m_paths.back();
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      INLINE MISC. FUNCTIONS...
    // *************************************************************************** //
    
    //  "maybe_snap"
    inline ImVec2                       maybe_snap                          (ImVec2 w) const
    { return m_grid.snap_on ? snap_to_grid(w) : w; }
    
    //  "path_is_mutable"
    inline bool                         path_is_mutable                     (const Path * p) const noexcept
    { return p && p->visible && !p->locked; }

    //  "_mode_has"
    inline bool                         _mode_has                           (CBCapabilityFlags flag) const
    { return ( this->ms_MODE_CAPABILITIES[ static_cast<size_t>(m_mode) ] & flag) != 0; }

    //  "_toggle_resident_overlay"
    inline void                         _toggle_resident_overlay            (const Resident idx) {
        auto & entry        = m_residents[idx];    Overlay * ov = entry.ptr;
        ov->info.visible    = !ov->info.visible;
        return;
    }

    //  "_set_resident_visibility"
    inline void                         _set_resident_visibility            (const Resident idx, const bool vis) {
        auto & entry        = m_residents[idx];    Overlay * ov = entry.ptr;
        ov->info.visible    = vis;
        return;
    }
    
    // *************************************************************************** //
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      INLINE CASUAL FUNCTIONS...
    // *************************************************************************** //
    inline bool                         has_file                            (void) const    { return ( std::filesystem::exists( m_editor_S.m_filepath ) ); }
    
    //  "PushVertexStyle"
    inline void                         PushVertexStyle                     (const VertexStyleType type) const noexcept
    { this->m_vertex_style.data = std::addressof( ms_VERTEX_STYLES[ type ] ); }
    
    //  "PopVertexStyle"
    inline void                         PopVertexStyle                      (void) const noexcept
    { this->m_vertex_style.data = std::addressof( ms_VERTEX_STYLES[ VertexStyleType::Default ] ); }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END INLINE FUNCITONS...






// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
};//    END "EDITOR" CLASS DEFINITION.












// *************************************************************************** //
//
//
//      SERIALIZATION...
// *************************************************************************** //
// *************************************************************************** //



// Extend similarly for Point, Line, GridSettings, ViewState …
//
struct EditorSnapshot
{
//      0.          CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
        _EDITOR_APP_INTERNAL_API
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr uint8_t            ms_MAJOR_VERSION                = EditorIMPL::ms_EDITOR_JSON_MAJ_VERSION;
    static constexpr uint8_t            ms_MINOR_VERSION                = EditorIMPL::ms_EDITOR_JSON_MIN_VERSION;
    
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
public:

    // *************************************************************************** //
    //      CORE DATA...
    // *************************************************************************** //
    std::vector<Vertex>                 vertices;
    std::vector<Path>                   paths;
    std::vector<Point>                  points;
    std::vector<Line>                   lines;
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  APPLICATION SUB-STATES...
    // *************************************************************************** //
    Selection                           selection;
    
    //                              SUBSIDIARY STATES:
    //  EditorState                         editor_S                        {   };        //  <======|    NEW CONVENTION.  Let's use "m_name_S" to denote a STATE variable...
    //  BrowserState                        m_browser_S                     {   };
    //  Selection                           m_sel;
    //  mutable BoxDrag                     m_boxdrag;
    //  MoveDrag                            m_movedrag;
    //  Clipboard                           m_clipboard;
    //
    //                              TOOL STATES:
    //  PenState                            m_pen;
    //  ShapeState                          m_shape;
    //  OverlayState                        m_overlay;
    //  DebuggerState                       m_debugger;
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                                  NEW STUFF...
    // *************************************************************************** //
    //  IndexState                          m_index_S                       {   };
    VertexID                            next_vid                        { 0 };            // first free vertex ID
    PathID                              next_pid                        { 0 };            // first free path  ID
    
    // *************************************************************************** //
    
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
    //                              DEFAULTED SPECIAL MEMBERS:
    inline                              EditorSnapshot          (const Editor & src);
    
    inline                              EditorSnapshot          (void)                              = default;
    inline                              EditorSnapshot          (const EditorSnapshot & )           = default;
    inline                              EditorSnapshot          (EditorSnapshot && ) noexcept       = default;
    //
    //                              DEFAULTED SPECIAL MEMBERS:
    EditorSnapshot &                    operator =              (const EditorSnapshot&)             = default;
    EditorSnapshot &                    operator =              (EditorSnapshot &&) noexcept        = default;
    
    // *************************************************************************** //
    //      DELETED FUNCTIONS.              |   ...
    // *************************************************************************** //
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".

    
   
// *************************************************************************** //
//
//
//      2.C         INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "copy_from"
    //      centralised copy function.
    //
    inline void                         copy_from               (const Editor & e)
    {
        //      1.      CORE DATA...
        this->vertices                  = e.m_vertices              ;
        this->paths                     = e.m_paths                 ;
        this->points                    = e.m_points                ;
        //  this->lines                     = e.m_lines                 ;
        //
        //
        //      2.      STATE OBJECTS...
        EditorStyle                         m_style                         {   };
        
        //
        //
        //      3.      SUB-STATE OBJECTS...
        this->selection                 = e.m_sel                   ;
        //
        //
        //      4.      ADDITIONAL MEMBERS...
        this->next_vid                  = e.m_next_id               ;       // << new
        this->next_pid                  = e.m_next_pid              ;       // << new
    
    
    
        return;
    }

    //  "assign_to"
    //      centralised "apply" function.
    //
    inline void                         assign_to               ([[maybe_unused]] const Editor & e)
    {
        //  1.  CORE DATA...
        //      e.m_vertices                    = std::vector<Vertex>   ( this->vertices  )     ;
        //      e.m_paths                       = std::vector<Point>    ( this->paths     )     ;
        //      e.m_points                      = std::vector<Line>     ( this->points    )     ;
        //      e.m_lines                       = std::vector<Path>     ( this->lines     )     ;
        //
        //
        //  2.  SUB-STATE OBJECTS...
        //      e.m_sel                         = Selection(this->selection)    ;
        //
        //
        //  3.  ADDITIONAL MEMBERS...
        //  this->next_vid                  = m_next_id;    // << new
        //  this->next_pid                  = m_next_pid;   // << new
    
        return;
    }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "EditorSnapshot" INLINE CLASS DEFINITION.




//  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EditorSnapshot,
//                                     version,
//                                     next_vid,
//                                     next_pid,
//                                     vertices,
//                                     paths,
//                                     points,
//                                     lines,
//                                     selection)




//  "to_json"
//
inline void to_json(nlohmann::json & j, const EditorSnapshot & s)
{
    j   = nlohmann::json{
        { "vertices",               s.vertices              },
        { "paths",                  s.paths                 },
        { "points",                 s.points                },
        { "selection",              s.selection             }
        //
        //  add grid / view / mode when you serialize them
        //
    };
    
    return;
}


//  "from_json"
//
inline void from_json(const nlohmann::json & j, EditorSnapshot & s)
{
    j.at(   "vertices"              )       .get_to (   s.vertices             );
    j.at(   "paths"                 )       .get_to (   s.paths                );
    j.at(   "points"                )       .get_to (   s.points               );
    j.at(   "selection"             )       .get_to (   s.selection            );
    
    return;
}

        

// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** // END "EditorSnapshot"












//  MORE STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "mode_label"
//
// static inline const char * mode_label(Mode m)
// { return Editor::ms_EDITOR_STATE_NAMES[ static_cast<size_t>(m) ]; }






//  NEW STATIC HELPERS...
// *************************************************************************** //
// *************************************************************************** //

//  "draw_vertex_slider"
//
inline bool s_draw_vertex_slider(const char * label, double & value, const double & range, const char * fmt = "%.3f")
{
    const bool      left_oob            = ( range < value );      //  "oob" = "OUT-OF-BOUNDS".
    const bool      right_oob           = ( range < value );
    bool            dirty               = false;
    
    dirty = ImGui::SliderScalar( label,
                                 ImGuiDataType_Double,
                                 &value,
                                 (left_oob)     ? &value        : &range ,
                                 (right_oob)    ? &value        : &range ,
                                 fmt
    );
    return dirty;
};



//  "draw_vertex_slider"
//
//  template < typename T, typename Param = cblib::math::Param<T> >
//  inline bool s_draw_vertex_slider(const char * label, double & value, const float speed, const Param & range, const char * fmt = "%.1f")
inline bool s_draw_vertex_slider(const char * label, double & value, const float speed, const double & min, const double & max, const char * fmt = "%.1f")
{
    constexpr ImGuiSliderFlags      flags               = ImGuiSliderFlags_None;
    constexpr int                   N                   = 1;
    bool                            dirty               = false;
        
        
    dirty = ImGui::DragScalarN(    label
                                 , ImGuiDataType_Double
                                 , &value
                                 , N
                                 , speed
                                 , &min
                                 , &max
                                 , fmt
                                 , flags
    );
    
    return dirty;
};
    
    
    






//  0.  STATIC INLINE       | DRAWING / CURVATURE / RENDERING...
// *************************************************************************** //
// *************************************************************************** //

//  "is_curved"
//
template< std::integral ID, typename Vertex = Vertex_t<ID> >
[[maybe_unused]] inline bool is_curved(const Vertex * a, const Vertex * b) noexcept
{
    return (a->m_bezier.out_handle.x || a->m_bezier.out_handle.y ||
            b->m_bezier.in_handle.x  || b->m_bezier.in_handle.y);
}


//  "cubic_eval"
//
template< std::integral ID, typename Vertex = Vertex_t<ID> >
[[maybe_unused]] static inline ImVec2 cubic_eval(const Vertex * a, const Vertex * b, float t)
{
    const float u  = 1.0f - t;
    const float w0 = u*u*u;
    const float w1 = 3*u*u*t;
    const float w2 = 3*u*t*t;
    const float w3 = t*t*t;

    ImVec2 P0{ a->x, a->y };
    ImVec2 P1{ a->x + a->m_bezier.out_handle.x, a->y + a->m_bezier.out_handle.y };
    ImVec2 P2{ b->x + b->m_bezier.in_handle.x,  b->y + b->m_bezier.in_handle.y  };
    ImVec2 P3{ b->x, b->y };

    return { w0*P0.x + w1*P1.x + w2*P2.x + w3*P3.x,
             w0*P0.y + w1*P1.y + w2*P2.y + w3*P3.y };
}


//  "point_in_polygon"
//
[[maybe_unused]]
static bool point_in_polygon(const std::vector<ImVec2> & poly, ImVec2 p)
{
    bool inside = false;
    const size_t n = poly.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++)
    {
        bool intersect = ((poly[i].y > p.y) != (poly[j].y > p.y)) &&
                         (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) /
                                 (poly[j].y - poly[i].y + 1e-6f) + poly[i].x);
        if (intersect) inside = !inside;
    }
    return inside;
}


//  "is_convex_poly"
//
[[maybe_unused]] static bool is_convex_poly(const std::vector<ImVec2> & poly)
{
    const size_t n = poly.size();
    if (n < 4) return true;
    float sign = 0.f;
    for (size_t i = 0; i < n; ++i) {
        const ImVec2& a = poly[i];
        const ImVec2& b = poly[(i + 1) % n];
        const ImVec2& c = poly[(i + 2) % n];
        float cross = (b.x - a.x) * (c.y - a.y) -
                      (b.y - a.y) * (c.x - a.x);
        if (cross != 0.f) {
            if (sign == 0.f) sign = cross;
            else if ((sign > 0.f) != (cross > 0.f)) return false;
        }
    }
    return true;
}
    
    




// *************************************************************************** //
//
//
//
//  1.  STATIC INLINE       | VECTOR OPERATIONS...
// *************************************************************************** //
// *************************************************************************** //

//  "vec_len"
//      Returns length of a 2-D vector.
//
static inline float vec_len(const ImVec2 & v) { return sqrtf(v.x*v.x + v.y*v.y); }


//  "vec_norm"
//      Normalises `v` unless zero; returns {0,0} if zero.
//
static inline ImVec2 vec_norm(const ImVec2 & v)
{
    float l = vec_len(v);
    return (l > 0.f) ? ImVec2{ v.x / l, v.y / l } : ImVec2{0,0};
}






// *************************************************************************** //
//
//
//
//  2.  STATIC INLINE       | MISC / OTHER...
// *************************************************************************** //
// *************************************************************************** //

//  "u32_to_f4"
//
[[maybe_unused]] static inline ImVec4 u32_to_f4(ImU32 c)        { return ImGui::ColorConvertU32ToFloat4(c); }


//  "f4_to_u32"
//
[[maybe_unused]] static inline ImU32  f4_to_u32(ImVec4 f)       { return ImGui::ColorConvertFloat4ToU32(f); }


//  "mirror_handles"
//      Mirrors the opposite handle according to anchor kind.
//
template< typename ID, typename Vertex = Vertex_t<ID> >
inline void mirror_handles(Vertex & v, bool dragged_out_handle) noexcept
{
    ImVec2 &        h_dragged       = (dragged_out_handle)  ? v.m_bezier.out_handle     : v.m_bezier.in_handle;
    ImVec2 &        h_other         = (dragged_out_handle)  ? v.m_bezier.in_handle      : v.m_bezier.out_handle;


    switch (v.m_bezier.kind)
    {
        case BezierCurvatureType::Other:
        {
            const float  len    = vec_len(h_other);          // preserve length
            const ImVec2 dir    = vec_norm(h_dragged);
            h_other             = ImVec2{ -dir.x * len, -dir.y * len };
            break;
        }

        case BezierCurvatureType::Cubic:
        {
            h_other = ImVec2{ -h_dragged.x, -h_dragged.y };
            break;
        }
        
        //  case BezierCurvatureType::None :
        default :                       //  { IM_ASSERT( false && "Function __func__ called with unknown AnchorType" ); }   //  Quiet the warning.
        {
            break;
        }
    }
    
    return;
}


//  "find_vertex_mut"
//
template< typename ID, typename Vertex = Vertex_t<ID> >
inline Vertex * find_vertex_mut( std::vector< Vertex > & arr, ID id ) noexcept
{
    for (Vertex & v : arr) {
        if (v.id == id)     { return &v; }
    }
        
    return nullptr;
}
//
template< typename ID, typename Vertex = Vertex_t<ID> >
inline const Vertex * find_vertex_mut( const std::vector< Vertex > & arr, ID id ) noexcept
{
    for (const Vertex & v : arr)
    {
        if (v.id == id)     { return std::addressof(v); }
    }
        
    return nullptr;
}


//  "alt_down"
//
static inline bool alt_down(void)
{
    ImGuiIO& io = ImGui::GetIO();
#if defined(ImGuiMod_Alt)          // 1.90+
    if (io.KeyMods & ImGuiMod_Alt) return true;
#endif
#if defined(ImGuiKey_ModAlt)       // some back-ends set this explicitly
    if (ImGui::IsKeyDown(ImGuiKey_ModAlt)) return true;
#endif
    if (io.KeyAlt)                 // 1.89 back-ends set this bool
        return true;
#if defined(ImGuiKey_LeftAlt)
    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt)  ||
        ImGui::IsKeyDown(ImGuiKey_RightAlt))
        return true;
#endif
    return false;
}



















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
}//   END OF "cb" NAMESPACE.






#endif      //  _CBWIDGETS_EDITOR_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
