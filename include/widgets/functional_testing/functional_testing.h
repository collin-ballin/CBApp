/***********************************************************************************
*
*       **************************************************************************
*       ****      F U N C T I O N A L _ T E S T I N G . H  ____  F I L E      ****
*       **************************************************************************
*
*              AUTHOR:      Collin A. Bond
*               DATED:      July 23, 2025.
*
*       ********************************************************************
*                FILE:      [./PyStream.h]
*
*
*
**************************************************************************************
**************************************************************************************/
#ifndef _CBWIDGETS_FUNCTIONAL_TESTING_H
#define _CBWIDGETS_FUNCTIONAL_TESTING_H  1



//  1.  INCLUDES    | Headers, Modules, etc...
// *************************************************************************** //
// *************************************************************************** //

//  0.1.        ** MY **  HEADERS...
#include CBAPP_USER_CONFIG
#include "cblib.h"
#include "utility/utility.h"
#include "widgets/functional_testing/_types.h"
//#include "app/titlebar/_titlebar.h"



//  0.2     STANDARD LIBRARY HEADERS...
#include <iostream>         //  <======| std::cout, std::cerr, std::endl, ...
#include <filesystem>
#include <system_error>
//
#include <cstdlib>          // C-Headers...
#include <stdio.h>
#include <unistd.h>


#include <string>           //  <======| std::string, ...
#include <string_view>
#include <format>
#include <vector>           //  <======| std::vector, ...
#include <stdexcept>        //  <======| ...
#include <limits.h>
#include <math.h>


#ifdef __CBAPP_DEBUG__      //  <======| Fix for issue wherein multiple instances of application
# include <thread>          //           are launched when DEBUG build is run inside Xcode IDE...
# include <chrono>
#endif     //  __CBAPP_DEBUG__  //



//  0.3     "DEAR IMGUI" HEADERS...
#include "json.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
# include <GLES2/gl2.h>
#endif      //  IMGUI_IMPL_OPENGL_ES2  //

#include <GLFW/glfw3.h>     //  <======| Will drag system OpenGL headers

//  [Win32]     Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
//                  - To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
//                  - Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
# pragma comment(lib, "legacy_stdio_definitions")
#endif

//              This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
# include "../../libs/emscripten/emscripten_mainloop_stub.h"
#endif





//  FORWARD DECLARATION...
//
namespace cb { namespace app { //     BEGINNING NAMESPACE "cb::app"...
// *************************************************************************** //
// *************************************************************************** //
    class       AppState;
    struct      WinInfo;

// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::app" NAMESPACE.






namespace cb { namespace ui { //     BEGINNING NAMESPACE "cb::ui"...
// *************************************************************************** //
// *************************************************************************** //



// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "ActionExecutor" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

struct ActionExecutor
{
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               State                           = ExecutionState;
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    static constexpr float              ms_MIN_DURATION_S               = 0.001f;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    //static constexpr auto &             ms_STATE_NAMES                  = DEF_MYCLASS_STATE_NAMES;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//  1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      IMPORTANT DATA-MEMBERS.
    // *************************************************************************** //
    GLFWwindow *                        m_window                        { nullptr };
    KeyHoldManager                      m_key_manager                   {  };
    Param<double>                       m_playback_speed                { 1.0f,  {0.10f, 4.0f} };
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      OTHER DATA MEMBERS.
    // *************************************************************************** //
    ImVec2                              m_first_pos                     {  };           //  starting mouse-cursor pos
    ImVec2                              m_last_pos                      {  };           //  destination mouse-cursor pos
    float                               m_duration_s                    { 0.0f };       //  total move time
    float                               m_elapsed_s                     { 0.0f };       //  accumulated time
    bool                                m_drag_button_left              { true };
    bool                                m_is_drag                       { false };   // ← NEW
    bool                                m_wait_one_frame                = false;
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATE VARIABLES.
    // *************************************************************************** //
    State                               m_state                         { State::None };
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//  2.A.            PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
                                        ActionExecutor          (void) noexcept     = default;
    explicit                            ActionExecutor          (GLFWwindow * window) noexcept;
                                        ~ActionExecutor         (void)              = default;
                                        
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    void                                start_cursor_move                   (GLFWwindow * window, ImVec2 first, ImVec2 last, float duration_s);
    //
    void                                start_mouse_click                   (GLFWwindow * window, bool left_button);
    void                                start_mouse_press                   (GLFWwindow * window, bool left_button);
    void                                start_mouse_release                 (GLFWwindow * window, bool left_button);
    void                                start_mouse_drag                    (GLFWwindow * window, ImVec2 from, ImVec2 to, float  dur_s, bool left_button);
    //
    void                                start_key_press                     (GLFWwindow * window, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super);
    void                                start_key_release                   (GLFWwindow * window, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super);
    //
    void                                start_button_action                 (GLFWwindow * window, ImGuiKey key, bool ctrl, bool shift, bool alt, bool super);
    //
    //
    //
    void                                abort                               (void);
    bool                                busy                                (void) const;
    void                                update                              (void);
    inline void                         reset                               (void)
    { m_key_manager.clear(); }
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".



// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //

    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //
    
    //  "_local_to_global"
    inline ImVec2       _local_to_global        (GLFWwindow * win, ImVec2 local)
    {
        int wx{}, wy{};
        glfwGetWindowPos(win, &wx, &wy);
        return { local.x + static_cast<float>(wx),
                 local.y + static_cast<float>(wy) };
    }
    
    //  "queue_mouse_button"
    inline void         queue_mouse_button      (int button, bool down)
    {
        ImGuiIO& io = ImGui::GetIO();
        bool prev   = io.AppAcceptingEvents;
        io.AppAcceptingEvents = true;          // force backend to accept
        io.AddMouseButtonEvent(button, down);
        io.AppAcceptingEvents = prev;
        return;
    };
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "ActionExecutor" STRUCT DEFINITION.









// *************************************************************************** //
// *************************************************************************** //
//                PRIMARY CLASS INTERFACE:
// 		Class-Interface for the "ActionComposer" Abstraction.
// *************************************************************************** //
// *************************************************************************** //

class ActionComposer
{
//  0.              CONSTANTS AND ALIASES...
// *************************************************************************** //
// *************************************************************************** //
public:

    // *************************************************************************** //
    //      NESTED TYPENAME ALIASES.
    // *************************************************************************** //
    using                               State                                       = ComposerState;
    using                               Composition                                 = Composition_t;
    CBAPP_CBLIB_TYPES_API
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //  0.2.                            BROWSER CONSTANTS...
    // *************************************************************************** //
    //                              BROWSER CHILD-WINDOW COLORS:
    ImVec4                              ms_CHILD_FRAME_BG1                          = ImVec4(0.205f,    0.223f,     0.268f,     1.000f);//      ms_CHILD_FRAME_BG1      //   BASE = #343944
    ImVec4                              ms_CHILD_FRAME_BG1L                         = ImVec4(0.091f,    0.099f,     0.119f,     0.800f);//      ms_CHILD_FRAME_BG1L     //   #17191E
    ImVec4                              ms_CHILD_FRAME_BG1R                         = ImVec4(0.129f,    0.140f,     0.168f,     0.800f);//      ms_CHILD_FRAME_BG1R     //   #21242B
    
    ImVec4                              ms_CHILD_FRAME_BG2                          = ImVec4(0.149f,    0.161f,     0.192f,     1.000f);//      ms_CHILD_FRAME_BG2      // BASE = #52596B
    ImVec4                              ms_CHILD_FRAME_BG2L                         = ImVec4(0.188f,    0.203f,     0.242f,     0.750f);//      ms_CHILD_FRAME_BG2L     // ##353A46
    ImVec4                              ms_CHILD_FRAME_BG2R                         = ImVec4(0.250f,    0.271f,     0.326f,     0.750f);//      ms_CHILD_FRAME_BG2R     // #5B6377
    //
    //                              BROWSER CHILD-WINDOW STYLE:
    float                               ms_VERTEX_SUBBROWSER_HEIGHT                 = 0.85f;    //  ms_VERTEX_SUBBROWSER_HEIGHT
    float                               ms_CHILD_BORDER1                            = 2.0f;     //  ms_CHILD_BORDER1
    float                               ms_CHILD_BORDER2                            = 1.0f;     //  ms_CHILD_BORDER2
    float                               ms_CHILD_ROUND1                             = 8.0f;     //  ms_CHILD_ROUND1
    float                               ms_CHILD_ROUND2                             = 4.0f;     //  ms_CHILD_ROUND2
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      STATIC CONSTEXPR CONSTANTS.
    // *************************************************************************** //
    //                              OVERLAY CONSTANTS:
    static constexpr ImVec2             ms_OVERLAY_OFFSET                           = ImVec2( 20.0f, 20.0f );
    static constexpr float              ms_OVERLAY_ALPHA                            = 0.65f;
    //
    //                              RENDERER HELPER CONSTANTS:
    static constexpr ImU32              ms_VIS_LINE_COLOR                           = utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   1.000f,     0.271f,     0.227f,     0.800f      )  );
    static constexpr ImU32              ms_VIS_COLOR_A                              = utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   0.196f,     0.843f,     0.294f,     1.000f      )  );
    static constexpr ImU32              ms_VIS_COLOR_B                              = utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   1.000f,     0.271f,     0.227f,     1.000f      )  );
    static constexpr float              ms_VIS_RECT_HALF                            = 8.0f;
    static constexpr float              ms_VIS_THICK                                = 3.0f;
    //
    //                              WIDGET CONSTANTS:
    static constexpr size_t             ms_COMPOSITION_NAME_LIMIT                   = 64ULL;
    static constexpr size_t             ms_COMPOSITION_DESCRIPTION_LIMIT            = 512ULL;
    static constexpr size_t             ms_ACTION_NAME_LIMIT                        = 64ULL;
    static constexpr size_t             ms_ACTION_DESCRIPTION_LIMIT                 = 256ULL;
    //
    static constexpr ImU32              ms_DELETE_BUTTON_COLOR                      = utl::ColorConvertFloat4ToU32_constexpr( ImVec4(   1.000f,     0.271f,     0.227f,     0.500f      )  );
    //
    //                              INDIVIDUAL WIDGET DIMENSIONS:
    static constexpr float              ms_COMPOSITION_DESCRIPTION_FIELD_HEIGHT     = 110.0f;
    static constexpr float              ms_ACTION_DESCRIPTION_FIELD_HEIGHT          = 65.0f;
    static constexpr ImVec2             ms_DELETE_BUTTON_DIMS                       = ImVec2( 18.0f, 0.0f );
    //
    //                              UI BROWSER DIMENSIONS:
    static constexpr float              ms_LABEL_WIDTH                              = 90.0f;
    static constexpr float              ms_WIDGET_WIDTH                             = 250.0f;
    //
    static constexpr ImVec2             ms_SETTINGS_BUTTON_SIZE                     = ImVec2( 55,   25 );
    static constexpr float              ms_SETTINGS_LABEL_WIDTH                     = 180.0f;
    static constexpr float              ms_SETTINGS_WIDGET_WIDTH                    = 300.0f;
    //
    //
    ImGuiChildFlags                     ms_COMPOSER_SELECTOR_FLAGS                  = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    Param<ImVec2>                       ms_COMPOSER_SELECTOR_DIMS                   = {     {400.0f, -1.0f},        { {150.0f, 1.0f},   {FLT_MAX, FLT_MAX} }   };
    //
    ImGuiChildFlags                     ms_ACTION_SELECTOR_FLAGS                    = ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX;
    Param<ImVec2>                       ms_ACTION_SELECTOR_DIMS                     = {     {500.0f, -1.0f},        { {200.0f, 1.0f},   {FLT_MAX, FLT_MAX} }   };
    //
    //  static constexpr float              ms_COMPOSITION_COLUMN_WIDTH             = 340.0f;
    //  static constexpr float              ms_SELECTOR_COLUMN_WIDTH                = 340.0f;
    static constexpr const char *       ms_DRAG_DROP_HANDLE                         = "=";
    static constexpr const char *       ms_DELETE_BUTTON_HANDLE                     = "-";
    static constexpr const char *       ms_EMPTY_HOTKEY_NAME                        = "UNASSIGNED";      // Used when Hotkey is UNASSIGNED...
    static constexpr const char *       ms_NO_ASSIGNED_FILE_STRING                  = "UNASSIGNED";      // Used when there is no "File > Save As..." assigned to app...
    //
    static constexpr float              ms_CONTROLBAR_SELECTABLE_SEP                = 16.0f;    //  controlbar offset.
    static constexpr float              ms_TOOLBAR_SELECTABLE_SEP                   = 16.0f;    //  sep for  "+ Add",  "Prev",  etc...
    static constexpr float              ms_BROWSER_SELECTABLE_SEP                   = 16.0f;    //  offset for buttons ontop of each selectable
    //
    //                              MISC. CONSTANTS:
    //                                  ...
    
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      REFERENCES TO GLOBAL ARRAYS.
    // *************************************************************************** //
    static constexpr auto &             ms_COMPOSER_STATE_NAMES                     = DEF_COMPOSER_STATE_NAMES;
    static constexpr auto &             ms_ACTION_TYPE_NAMES                        = DEF_ACTION_TYPE_NAMES;
    static constexpr auto &             ms_EXEC_STATE_NAMES                         = DEF_EXEC_STATE_NAMES;
    static constexpr auto &             ms_CLICK_PARAM_NAMES                        = DEF_MOUSE_CLICK_TYPES_NAMES;
    static constexpr auto &             ms_CLICK_TYPE_NAMES                         = DEF_CLICK_TYPE_NAMES;
    
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CONSTANTS AND ALIASES".



// *************************************************************************** //
//
//
//  1.              CLASS DATA-MEMBERS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    //                              IMPORTANT DATA:
    app::AppState &                     CBAPP_STATE_NAME;
    std::unique_ptr<app::WinInfo>       m_detview_window;
    bool                                m_initialized                               = false;
    //
    ActionExecutor                      m_executor                                  {  };
    std::vector<Composition>            m_compositions                              { 1 };
    std::vector<Action> *               m_actions                                   = nullptr;
    std::filesystem::path               m_filepath                                  = {"/Users/collinbond/Desktop/HOME_DIRECTORY/10_PROJECTS/imgui/CBApp/assets/.cbapp/debug/editor_tests_1.json"};
    //
    int                                 m_comp_sel                                  = 0;            // current composition selection
    int                                 m_sel                                       = -1;           // current action selection
    int                                 m_play_index                                = -1;           // current action being executed, -1 = idle
    //
    //                              STATE:
    State                               m_state                                     = State::Idle;
    //
    //                              SUB-STATES:
    KeyCaptureState                     m_key_capture                               = {  };
    MouseCaptureState                   m_mouse_capture                             = {  };
    OverlayCache                        m_overlay_cache                             = {  };
    //
    //                              SETTINGS VARIABLES:
    bool                                m_show_overlay                              = true;
    bool                                m_render_visuals                            = true;
    bool                                m_allow_input_blocker                       = true;         //  DRAWS THE WINDOW THAT BLOCKS INPUT...
    bool                                m_show_composition_browser                  = true;
    bool                                m_show_composition_inspector                = false;
    //
    //                              MUTABLE STATE VARIABLES:
    bool                                m_is_running                                = false;
    bool                                m_step_req                                  = false;
    bool                                m_capture_is_active                         = false;        //  < true while “Auto” sampling.
    bool                                m_key_capture_is_active                     = false;        //  < true while “Auto” sampling.
    //
    bool                                m_saving                                    = false;
    bool                                m_loading                                   = false;
    //
    //
    //                              UTILITY:
    ImVec2                              m_overlay_size                              = { 150.f, 30.f };
    GLFWmonitor *                       m_active_monitor                            = nullptr;
    ImRect                              m_monitor_bounds                            {  };
    ImGuiTextFilter                     m_filter;
    ImVec2 *                            m_m_capture_dest                            = nullptr;      //  < pointer to coord being written
    //HotkeyParams *                      m_k_capture_key_dest                          = nullptr;
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "CLASS DATA-MEMBERS".



// *************************************************************************** //
//
//
//  2.A.            PUBLIC MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
public:
    
    // *************************************************************************** //
    //      INITIALIZATION METHODS.         |   "init.cpp" ...
    // *************************************************************************** //
    explicit                            ActionComposer          (app::AppState & src);
                                        ~ActionComposer         (void)      = default;
    //
    //                              INITIALIZATION METHODS:
    void                                initialize              (void);
    // *************************************************************************** //
    //
    //
    // *************************************************************************** //
    //      MAIN API.                       |   "interface.cpp" ...
    // *************************************************************************** //
    void                                Begin                               ([[maybe_unused]] const char *,     [[maybe_unused]] bool *,    [[maybe_unused]] ImGuiWindowFlags);
    void                                draw_all                            (void);
protected:
    void                                Begin_IMPL                          (void);
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PUBLIC MEMBER FUNCS".


    
// *************************************************************************** //
//
//
//      2.B.        PROTECTED MEMBER FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:
    
    // *************************************************************************** //
    //      MAIN UI FUNCTIONS...
    // *************************************************************************** //
    //                              COMPOSITION UI:
    void                                _draw_composition_selector          (void);
    void                                _draw_composition_table             (void);
    void                                _draw_composition_inspector         (Composition & );
    //
    //                              ACTION UI:
    void                                _draw_action_selector               (void);
    void                                _draw_action_table                  (void);
    void                                _draw_action_inspector              (void);
    //
    //                              OTHER:
    void                                _draw_renderer_visuals              (void);
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      OVERLAY UI CONTENT.             |   "interface.cpp" ...
    // *************************************************************************** //
    void                                _draw_controlbar                    (void);
    void                                _draw_toolbar                       (void);
    void                                _draw_overlay                       (void);
    //
    //                              OVERLAY CONTENT:
    void                                _dispatch_overlay_content           (void);
    void                                _overlay_ui_none                    (void);
    void                                _overlay_ui_run                     (void);
    void                                _overlay_ui_mouse_capture           (void);
    void                                _overlay_ui_key_capture             (void);
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      GENERAL FUNCTIONS.              |   "action.cpp" ...
    // *************************************************************************** //
    void                                _drive_execution                    (void);
    inline void                         _dispatch_execution                 (Action & act);
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      ACTION-UI FUNCTIONS.            |   "action.cpp" ...
    // *************************************************************************** //
    void                                _dispatch_action_ui                 (Action & a);
    inline void                         _ui_cursor_move                     (Action & a);
    //
    inline void                         _ui_mouse_click                     (Action & a);
    inline void                         _ui_mouse_press                     (Action & a);
    inline void                         _ui_mouse_release                   (Action & a);
    inline void                         _ui_mouse_drag                      (Action & a);
    //
    inline void                         _ui_hotkey                          (Action & a);
    //
    //                              GENERIC UI FUNCTIONS:
    inline void                         _ui_movement_widgets                (Action & a);
    inline void                         _ui_duration_widgets                (Action & a);
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      CAPTURE FUNCTIONS...
    // *************************************************************************** //
    //                              CURSOR CAPTURE:
    ImVec2                              get_cursor_pos                      (void);
    bool                                _begin_mouse_capture                ([[maybe_unused]] Action & act, ImVec2 * destination);
    inline void                         _update_mouse_capture               (void);
    //
    //                              KEYBOARD CAPTURE:
    bool                                _begin_key_capture                  (HotkeyParams * dest);
    inline void                         _update_key_capture                 (void);
    inline void                         _accept_key_capture                 (void);
    inline void                         _cancel_key_capture                 (void);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      UTILITY FUNCTIONS...
    // *************************************************************************** //
    void                                _draw_settings_menu                 (void);
    void                                _draw_input_blocker                 (void);
    void                                _refresh_monitor_cache              (ImVec2);
    //
    //                              SERIALIZATION:
    void                                _file_dialog_handler                (void);
    bool                                save_to_file                        (const std::filesystem::path & path) const;
    bool                                save_to_file_IMPL                   (const std::filesystem::path & path) const;
    bool                                load_from_file                      (const std::filesystem::path & path);
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED FUNCTIONS.          |   "common.cpp" ...
    // *************************************************************************** //
    //                              CENTRALIZED COMPOSITION FUNCTIONS:
    void                                _delete_composition                 (const int index);
    void                                _duplicate_composition              (const int index);
    //
    //                              CENTRALIZED ACTION FUNCTIONS:
    void                                _delete_action                      (const int index);
    void                                _duplicate_action                   (const int index);
    
    
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "PROTECTED" FUNCTIONS.

    
   
// *************************************************************************** //
//
//
//  2.C                 INLINE FUNCTIONS...
// *************************************************************************** //
// *************************************************************************** //
protected:


    // *************************************************************************** //
    //      CENTRALIZED OPERATION FUNCTIONS.
    // *************************************************************************** //
    
    inline void                         _run_all                        (void) {
        this->m_executor.reset();
        this->m_sel         = -1;
        m_play_index        = (m_sel >= 0               ? m_sel             : 0);
        m_is_running        = !m_actions->empty();
        m_state             = (m_actions->empty())      ? State::Idle       : State::Run;
        return;
    }

    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      CENTRALIZED STATE MANAGEMENT FUNCTIONS.
    // *************************************************************************** //

    //  "is_running"
    inline bool                         is_running                      (void) const    { return ( this->m_is_running  &&  (this->m_state == State::Run) ); }
    
    //  "reset_state"
    inline void                         reset_state                     (void) {
        m_key_capture                       .reset();
        m_mouse_capture                     .reset();
        m_overlay_cache                     .reset();
        //
        m_state                             = State::Idle;
        //  this->m_sel                         = -1;
        //  this->m_play_index                  = -1;
        m_is_running                        = false;
        m_capture_is_active                 = false;
        m_key_capture_is_active             = false;
        m_show_composition_inspector        = false;
        //
        m_executor                          .reset();
        return;
    }
    
    //  "reset_data"
    inline void                         reset_data                      (void)          {
        m_sel                       = -1;
        m_comp_sel                  = -1;
        m_play_index                = -1;
        m_m_capture_dest            = nullptr;
        m_executor.abort();
        return;
    }
    
    //  "abort_test"
    inline void                         abort_test                      (void)           {
        //m_sel               = -1;        // no action selected
        this->exit_test();
        m_executor          .abort();
        m_key_capture       .reset();
        return;
    }
    
    //  "exit_test"
    inline void                         exit_test                       (void)           {
        //  m_step_req   = false;
        //  reset_state();
        //  m_play_index = -1;
        reset_state();
        m_step_req          = false;
        m_play_index        = -1;
        return;
    }
    
    //  "reset_all"
    inline void                         reset_all                       (void)
    { this->reset_state();    this->reset_data(); }
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //                      CENTRALIZED OPERATION FUNCTIONS...
    // *************************************************************************** //
        
    //  "_load_actions_from_comp"
    //
    inline void                         _load_actions_from_comp         (int comp_index)
    {
        if ( m_compositions.empty() )   { return; }

        comp_index              = std::clamp( comp_index, 0, static_cast<int>(m_compositions.size()) - 1 );
        this->reset_all();
        
        m_comp_sel              = comp_index;
        m_actions               = &m_compositions[m_comp_sel].actions;
        return;
    }


    //  "_save_actions_to_comp"
    //
    inline void                         _save_actions_to_comp           (void) {
        m_compositions[m_comp_sel].actions = *m_actions;
        return;
    }
    
    
    //  "_reorder"
    inline void                         _reorder                        (int from, int to)
    {
        if( from == to )    { return; }
        
        Action      tmp         = std::move( (*m_actions)[from] );
        m_actions->erase( m_actions->begin() + from );
        m_actions->insert( m_actions->begin() + to, std::move(tmp) );
        m_sel                   = to;
        
        return;
    }
    
    //  "_reorder_composition"
    inline void                         _reorder_composition            (int from, int to)
    {
        if (from == to) { return; }

        /* persist edits to currently open composition before we shuffle */
        _save_actions_to_comp();

        /* move the element -------------------------------------------------- */
        Composition_t tmp = std::move(m_compositions[static_cast<size_t>(from)]);
        m_compositions.erase(m_compositions.begin() + from);
        m_compositions.insert(m_compositions.begin() + to, std::move(tmp));

        /* update selection & action pointer -------------------------------- */
        m_comp_sel = to;
        _load_actions_from_comp(m_comp_sel);      // resets m_actions to new slot
    }
    
    //  "need_step"
    inline bool                         need_step                       (void) const    { return m_step_req; }
    
    //  "has_file"
    inline bool                         has_file                        (void) const    { return ( std::filesystem::exists( m_filepath ) ); }
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      MISC. UTILITY FUNCTIONS.
    // *************************************************************************** //
    
    //  "label"
    inline void                         label                           (const char * text, const float l_width=ms_LABEL_WIDTH, const float w_width=ms_WIDGET_WIDTH)
    { utl::LeftLabel(text, l_width, w_width); ImGui::SameLine(); };
    
    
    
    // *************************************************************************** //
    //
    //
    //
    // *************************************************************************** //
    //      PUBLIC INLINE UTILITY FUNCTIONS...
    // *************************************************************************** //
public:

    //  "get_detview_window"
    [[nodiscard]]
    inline app::WinInfo *               get_detview_window              (void)
    {  return this->m_detview_window.get(); }
    
    
    
    // *************************************************************************** //
    
//
//
//
// *************************************************************************** //
// *************************************************************************** //   END "INLINE" FUNCTIONS.






// *************************************************************************** //
// *************************************************************************** //
};//	END "ActionComposer" INLINE CLASS DEFINITION.















// *************************************************************************** //
//
//
//
// *************************************************************************** //
// *************************************************************************** //
} } //   END OF "cb::ui" NAMESPACE.






#endif      //  _CBWIDGETS_FUNCTIONAL_TESTING_H  //
// *************************************************************************** //
// *************************************************************************** //
//
//  END.
